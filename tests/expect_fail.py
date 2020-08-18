#encoding: utf-8
#vim: sw=4 ts=4 noexpandtab

import subprocess, argparse, sys, os, platform

parser = argparse.ArgumentParser(
		description='Expect a test to fail with a certain exit code')

parser.add_argument('--cmd', dest='cmd', action='store', default=None)
parser.add_argument('--expect', dest='expect', action='store', default=None)

args = parser.parse_args()

if not args.cmd or not args.expect:
	sys.exit(-8)

if platform.system() == 'Windows' and not args.cmd.endswith('.exe'):
	args.cmd += '.exe'

proc = subprocess.Popen(
	args.cmd,
	cwd=os.getcwd(),
	stderr=subprocess.PIPE,
	stdout=subprocess.PIPE)

(stdout, stderr) = proc.communicate()

if stdout:
	print(stdout)

if stderr:
	print(stderr, file=sys.stderr)

if proc.returncode == int(args.expect):
	sys.exit(0)
elif proc.returncode == 0:
	print('Failing test exited with 0 erroneously!', file=sys.stderr)
	sys.exit(-7)
else:
	sys.exit(proc.returncode)
