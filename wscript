#!/usr/bin/env python3
# encoding: utf-8
# vim: sw=4 ts=4 noexpandtab

import sysconfig

from waflib import Logs, Options
from waflib.Build import BuildContext
from waflib.TaskGen import feature, after_method, before_method
from waflib.Tools import waf_unit_test
from waflib.Tools.ccroot import USELIB_VARS

top = '.'
out = 'build'

run_tests = False

def options(opt):
	Logs.enable_colors(2)

	opt.load('clang_cl') # Also loads msvc
	opt.load('clang_compilation_database')
	opt.load('waf_unit_test')
	opt.parser.remove_option('--alltests')
	opt.parser.remove_option('--notests')
	opt.parser.remove_option('--clear-failed')

	opt.add_option(
		'--clangdb-release',
		action='store_true',
		dest='clangdb_conf',
		default=False,
		help="Use release flags in the clang compilation database.")

	opt.add_option(
		'--use-msvc',
		action='store_true',
		dest='use_msvc',
		default=False,
		help="Use MSVC instead of clang-cl (temporary until I add GCC support).")
#enddef

def configure(cfg):
	USELIB_VARS['c'].add('SYSINCLUDES')
	USELIB_VARS['cxx'].add('SYSINCLUDES')

	Logs.enable_colors(2)

	cfg.setenv('debug')
	if cfg.options.use_msvc:
		cfg.load('msvc msvc_pdb')
	else:
		cfg.load('clang_cl')
	#endif
	cfg.load('clang_compilation_database')
	cfg.env.cstlib_PATTERN = cfg.env.cxxstlib_PATTERN = 'lib%s.lib'
	cfg.env.STLIB_ST = 'lib%s.lib'

	cfg.load('nasm')
	as_platform = ['-f', 'win64', '-m', 'amd64']

	if cfg.env.DEST_CPU == 'x86':
		as_platform = ['-f', 'win32', '-m', 'x86']
	#endif

	asflags = ['-g', 'cv8'] + as_platform
	cfg.env.append_value('ASFLAGS', asflags)

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
			'/permissive-', # Enable standard conformance mode
			'/volatile:iso', # Disable volatile as atomic
			'/wd5045', # Disable if /Qspectre
			'/wd4820', # Disable X bytes padding added after data member Y
			'/guard:cf' # Enable control flow guard
		]

	if not ('CC_NAME_SECONDARY' in cfg.env):
		flags += \
			[
				'/experimental:external',
				'/external:W0',
				'/external:env:INCLUDE', # Stuff in the include env var should not gen warnings
			]

		if cfg.env.DEST_CPU != 'amd64':
			flags += ['/arch:IA32']
		#endif
	else:
		flags += \
			[
				'-Wno-c++98-compat',
				'-Wno-c++98-compat-pedantic',
				'-Wno-main',
				'-Wno-reserved-id-macro',
				'-Wno-gnu-include-next'
			]

		if cfg.env.DEST_CPU == 'amd64':
			flags += ['-m64', '-march=x86-64']
		else:
			flags += ['-m32', '-march=pentium']
		#endif
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
			'/EHsc'
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

	cfg.env.LDFLAGS = ['/DEBUG', '/INCREMENTAL:NO', '/WX', '/GUARD:CF']

	cfg.recurse('LotusCRT')
	cfg.recurse('LotusStdC')

	if cfg.env.DEST_CPU == 'amd64':
		cfg.env.append_value('LDFLAGS', '/MACHINE:X64')
		cfg.env.append_value('ARFLAGS', '/MACHINE:X64')
	else:
		cfg.env.append_value('LDFLAGS', '/MACHINE:X86')
		cfg.env.append_value('ARFLAGS', '/MACHINE:X86')
	#endif

	cfg.setenv('release', env=cfg.env.derive())
	release_flags = \
		[
			'-O2',
			'-Ob2',
			'-Oi',
			'-Ot',
			'-Oy',
			'-Gy',
			'-fp:fast',
			'-wd4711' # Function selected for automatic inline expansion
		]

	debug_flags = \
		[
			'-Od',
			'-Ob0',
			'-Oi',
			'-Oy-',
			'-Gy-',
			'-fp:precise'
		]

	if not 'CC_NAME_SECONDARY' in cfg.env:
		release_flags += ['-GL']
		debug_flags += ['-GL-']
		cfg.env.append_value('LDFLAGS','/LTCG')
		cfg.env.append_value('ARFLAGS', '/LTCG')

		if cfg.env.DEST_CPU == 'amd64':
			cfg.env.append_value(
				'LDFLAGS',
				[
					'/INCLUDE:__guard_dispatch_icall_fptr',
					'/INCLUDE:_load_config_used'
				])
		#endif
	else:
		release_flags += ['-Xclang', '-O3']
	#endif

	cfg.env.append_value('CFLAGS', release_flags)
	cfg.env.append_value('CXXFLAGS', release_flags)
	cfg.env.append_value('DEFINES', '__LOTUSCRT_RELEASE')
	cfg.env.append_value('LDFLAGS', '/OPT:REF,ICF')

	cfg.setenv('debug')
	cfg.env.append_value('CFLAGS', debug_flags)
	cfg.env.append_value('CXXFLAGS', debug_flags)
	cfg.env.append_value('DEFINES', '__LOTUSCRT_DEBUG')
#enddef

def summary(bld):
	lst = getattr(bld, 'utest_results', [])
	if lst:
		total = len(lst)
		tfail = len([x for x in lst if x[1]])

		val = 100 * (total - tfail) / (1.0 * total)
		Logs.pprint('CYAN', 'Test report: %3.0f%% success' % val)

		Logs.pprint(
			'CYAN',
			'  Tests that succeed: %d/%d' % (total - tfail, total))

		Logs.pprint('CYAN', '  Tests that fail: %d/%d' % (tfail, total))

		for result in lst:
			if result.exit_code:
				Logs.pprint(
					'CYAN',
					'    %s (%s)' % (result.test_path, result.generator.name))

				Logs.pprint('RED', 'Status: %r' % result.exit_code)
				if result.out: Logs.pprint('RED', 'out: %r' % result.out)
				if result.err: Logs.pprint('RED', 'err: %r' % result.err)
			#endif
		#endfor
	#endif
#enddef

def build(bld):
	USELIB_VARS['c'].add('SYSINCLUDES')
	USELIB_VARS['cxx'].add('SYSINCLUDES')

	Logs.enable_colors(2)

	if not bld.variant:
		if not bld.cmd == 'clangdb' and not bld.cmd == 'clean':
			Options.commands.insert(0, 'build_release')
			Options.commands.insert(0, 'build_debug')
			return
		else:
			# Clang compilation database module does not play nice with variants.
			# Set the variant to debug, or release by user override.
			bld.variant = 'release' if bld.options.clangdb_conf else 'debug'
		#endif
	#endif

	global run_tests
	if run_tests:
		bld.options.all_tests = True
		bld.options.no_tests = False
	else:
		bld.options.all_tests = False
		bld.options.no_tests = True
	#endif

	bld.options.clear_failed_tests = True
	bld.add_post_fun(summary)
	bld.add_post_fun(waf_unit_test.set_exit_code)

	bld.recurse('LotusCRT')
	bld.recurse('LotusStdC.Memory')
	bld.recurse('LotusStdC.String')
	bld.recurse('tests')
#enddef

def test(bld):
	Logs.enable_colors(2)

	global run_tests
	run_tests = True

	Options.commands = ['build'] + Options.commands
#enddef

class TestContext(BuildContext):
	'''Build and execute unit tests'''
	cmd = 'test'
	fun = 'test'
#endclass

def init(ctx):
	from waflib.Build import BuildContext, CleanContext, InstallContext
	from waflib.Build import UninstallContext

	for x in 'debug release'.split():
		for y in (
			TestContext,
			BuildContext,
			CleanContext,
			InstallContext,
			UninstallContext):

			name = y.__name__.replace('Context', '').lower()

			class tmp(y):
				cmd = name + '_' + x
				variant = x
			#endclass
		#endfor
	#endfor
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
