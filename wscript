#!/usr/bin/env python3
# encoding: utf-8
# vim: sw=4 ts=4 noexpandtab

from waflib.TaskGen import feature, after_method, before_method
from waflib.Tools.ccroot import USELIB_VARS

top = '.'
out = 'build'

def options(opt):
	#opt.load('msvc')
	opt.load('clang_cl clang_compilation_database')

#enddef

def configure(cfg):
	USELIB_VARS['c'].add('SYSINCLUDES')
	USELIB_VARS['cxx'].add('SYSINCLUDES')

	#cfg.load('msvc msvc_pdb')
	cfg.load('clang_cl clang_compilation_database')
	cfg.env.cstlib_PATTERN = cfg.env.cxxstlib_PATTERN = 'lib%s.lib'
	cfg.env.STLIB_ST = 'lib%s.lib'

	def keep_winsdk(cur):
		return not (
			('ATLMFC' in cur)
			or ('MSVC' in cur)
			or ('NETFXSDK' in cur)
			or ('ucrt' in cur)
			or ('cppwinrt' in cur)
			or ('winrt' in cur)
			or ('Microsoft.NET' in cur))
	#enddef

	cfg.env.SYSINCLUDES_MSVC = list(filter(keep_winsdk, cfg.env.INCLUDES))
	cfg.env.INCLUDES = []
	cfg.env.LIBPATH = list(filter(keep_winsdk, cfg.env.LIBPATH))
	flags = \
		[
			'/WX', # Warnings are errors
			'/Wall', # Enable all warnings
			'/X', # Ignore default includes
			'/Zc:inline', # Enable removal of unreferenced data
			'/Zc:wchar_t', # Enable wchar_t as a built-in type
			'/Zi', # Enable debug info
			'/Zl', # Stop the compiler from inserting references to MSVCRT and oldnames
			#'/arch:SSE2', # Enable SSE2 codegen
			'/permissive-', # Enable standard conformance mode
			'/volatile:iso', # Disable volatile as atomic
			'/Od',
			'/Ob0'
		]

	if not ('CC_NAME_SECONDARY' in cfg.env):
		flags += \
			[
				'/experimental:external',
				'/external:W0',
				'/external:env:INCLUDE', # Stuff in the include env var should not gen warnings
			]
	else:
		flags += \
			[
				'-Wno-c++98-compat',
				'-Wno-c++98-compat-pedantic',
				'-Wno-main',
				'-Wno-reserved-id-macro',
				'-m64'
			]
	#endif

	cfg.env.append_value('CFLAGS', flags)
	cfg.env.append_value('CXXFLAGS', flags)

	cfg.env.append_value(
		'CXXFLAGS',
		[
			'/Zc:__cplusplus',
			'/Zc:auto',
			'/Zc:rvalueCast',
			'/Zc:strictStrings',
			'/Zc:threadSafeInit',
		])

	if 'CC_NAME_SECONDARY' in cfg.env:
		cfg.env.SYSINCFLAG = '/imsvc'
	else:
		cfg.env.SYSINCFLAG = '/external:I'
		cfg.env.append_value(
			'CXXFLAGS',
			[
				'/Zc:externConstexpr',
				'/Zc:implicitNoexcept',
				'/Zc:referenceBinding',
				'/Zc:throwingNew'
			])
	#endif

	cfg.env.LDFLAGS = ['/DEBUG', '/INCREMENTAL:NO', '/WX']

	cfg.recurse('LotusCRT')
	cfg.recurse('LotusStdC')
#enddef

def build(bld):
	USELIB_VARS['c'].add('SYSINCLUDES')
	USELIB_VARS['cxx'].add('SYSINCLUDES')

	bld.recurse('LotusCRT')
	bld.recurse('LotusStdC.Memory')
#enddef

@feature('c', 'cxx', 'system_includes')
@after_method('propagate_uselib_vars', 'process_source', 'apply_incpaths')
@before_method('add_pdb_per_object')
def apply_sysinc(self):
	nodes = self.to_incnodes(
		self.to_list(getattr(self, 'system_includes', []))
		+ self.env.SYSINCLUDES)

	self.includes_nodes += nodes
	cwd = self.get_cwd()

	if self.env.SYSINCFLAG:
		for node in nodes:
			self.env.append_value(
				'CFLAGS',
				[self.env.SYSINCFLAG] + [node.path_from(cwd)])

			self.env.append_value(
				'CXXFLAGS',
				[self.env.SYSINCFLAG] + [node.path_from(cwd)])
		#endfor
	else:
		self.env.INCPATHS = [x.path_from(cwd) for x in nodes]
	#endif
#enddef

@feature('c', 'cxx', 'use')
@before_method('apply_incpaths', 'propagate_uselib_vars')
@after_method('process_use')
def process_extra_use(self):
	#Process the ``use`` attribute which contains a list of task generator names::
	#
	#	def build(bld):
	#		bld.shlib(source='a.c', target='lib1')
	#		bld.program(source='main.c', target='app', use='lib1')
	#
	#See :py:func:`waflib.Tools.ccroot.use_rec`.

	#from waflib.Tools import ccroot

	use_not = self.tmp_use_not = set()
	self.tmp_use_seen = [] # we would like an ordered set
	use_prec = self.tmp_use_prec = {}
	self.system_includes = self.to_list(getattr(self, 'system_includes', []))
	self.force_includes = self.to_list(getattr(self, 'force_includes', []))
	names = self.to_list(getattr(self, 'use', []))

	for x in names:
		self.use_rec(x)
	#endfor

	for x in use_not:
		if x in use_prec:
			del use_prec[x]
		#endif
	#endfor

	# topological sort
	out = self.tmp_use_sorted = []
	tmp = []
	for x in self.tmp_use_seen:
		for k in use_prec.values():
			if x in k:
				break
			#endif
		else:
			tmp.append(x)
		#endfor
	#endfor

	while tmp:
		e = tmp.pop()
		out.append(e)
		try:
			nlst = use_prec[e]
		except KeyError:
			pass
		else:
			del use_prec[e]
			for x in nlst:
				for y in use_prec:
					if x in use_prec[y]:
						break
					#endif
				else:
					tmp.append(x)
				#endfor
			#endfor
		#endtry
	#endwhile

	if use_prec:
		raise Errors.WafError('Cycle detected in the use processing %r'
			% use_prec)
	#endif
	out.reverse()

	for x in out:
		y = self.bld.get_tgen_by_name(x)

		if getattr(y, 'export_system_includes', None):
			# self.system_includes may come from a global variable #2035
			self.system_includes = self.system_includes \
				+ y.to_incnodes(y.export_system_includes)
		#endif

		if getattr(y, 'export_force_includes', None):
			self.force_includes = self.force_includes \
				+ y.to_nodes(y.export_force_includes)
		#endif
	#endfor
#enddef
