#!/usr/bin/python
import os
import sys
import re
import argparse
import subprocess
import pexpect

parser = argparse.ArgumentParser(description='Execute a crab command on all files in a directory')
parser.add_argument("dir", help="Directory that contains the crab task directories [Default: crab]")
parser.add_argument("com", help="Command to run")
parser.add_argument("-s", "--sel", dest="sel", default=".+", help="Selection for tasks to run on in regex for anything after 'crab_' [Default: .+]")
parser.add_argument("-o",dest="options", default="", help="crab command optional arguments to run")
parser.add_argument("-d", "--dryRun",  dest="dryRun", action='store_true', default=False, help="Set to just show the commands but not execute. [Default: False]")
args = parser.parse_args()

cmd = ("find %s -maxdepth 1 -type d -name \"crab_*\" | egrep \"(%s/*crab_%s)\"" % (args.dir,args.dir, args.sel))
ps = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
result = ps.communicate()
filelist = result[0].rstrip('\n').split('\n')

for filename in filelist :
	cC = "crab " +  args.com + " " + args.options + " " + filename
	print "---------"
	print cC
	if not args.dryRun:
		child = pexpect.spawn(cC) #use pexpect for colors
		child.expect(pexpect.EOF)
		print(child.before)
# 		ps2 = subprocess.Popen(cC +" 2>&1", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
# 		result = ps2.communicate()
# 		print result[0]
		

		
