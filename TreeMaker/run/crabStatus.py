#!/usr/bin/python
import os
import sys
import re
import argparse
import subprocess
import pexpect

parser = argparse.ArgumentParser(description='Execute a crab command on all files in a directory')
parser.add_argument("dir", help="Directory that contains the crab task directories [Default: crab]")
parser.add_argument("-s", "--sel", dest="sel", default=".+", help="Selection for tasks to run on in regex for anything after 'crab_' [Default: .+]")
args = parser.parse_args()

cmd = ("find %s -maxdepth 1 -type d -name \"crab_*\" | egrep \"(%s/*crab_%s)\"" % (args.dir,args.dir, args.sel))
ps = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
result = ps.communicate()
filelist = result[0].rstrip('\n').split('\n')

def findType(line,name) :
	mNum = re.match("^\s+(\S+)\s+\d.* \(\s*(\d+)/\d+\).*", line)
	if mNum and mNum.group(1) == name  :
		return int(mNum.group(2))
	else :
		return 0
	

maxSize = 12
for filename in filelist :
	match = re.match(("^%s/*crab_(.+).*"%(args.dir)), filename)
	if not match:
		continue
	if len(match.group(1)) + 4 > maxSize :
		maxSize = len(match.group(1)) + 4 
		
pTitle = "{m: <{w}}".format(m="Sample name",w=maxSize)
pTitle += "{m: <{w}}".format(m="Total",w=10)
pTitle += "\033[36m{m: <{w}}\033[0m".format(m="Finished",w=10)
pTitle += "\033[93m{m: <{w}}\033[0m".format(m="Running",w=10)
pTitle += "\033[91m{m: <{w}}\033[0m".format(m="Failed",w=10)
pTitle += "{m: <{w}}".format(m="Idle",w=10)
pTitle += "{m: <{w}}".format(m="Unsub.",w=10)
pTitle += "{m: <{w}}".format(m="Other",w=10)
print(pTitle)

for filename in filelist :
	match = re.match(("^%s/*crab_(.+).*"%(args.dir)), filename)
	if not match:
		print "Do not understand: " + filename
		continue
	name = match.group(1)
	cC = "crab status " + filename 
	ps2 = subprocess.Popen(cC, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	result2 = ps2.communicate()
	pRes = result2[0].rstrip('\n').split('\n')
	
	nTotal = 0
	nFinished = 0
	nFailed = 0
	nIdle = 0
	nUnSub = 0
	nRunning = 0
	for line in pRes:
		if nTotal > 0 :
			nFinished += findType(line,"finished")
			nFailed += findType(line,"failed")
			nIdle += findType(line,"idle")
			nUnSub += findType(line,"unsubmitted")
			nRunning += findType(line,"running")
			continue
		mTotal = re.match("^Jobs status:\s+(\S+)\s+\d.* \(\s*(\d+)/(\d+)\).*", line)
		if mTotal:
			nTotal = int(mTotal.group(3))
			if mTotal.group(1)  == "finished" :
				nFinished += int(mTotal.group(2))
			elif mTotal.group(1)  == "failed" :
				nFailed += int(mTotal.group(2))
			elif mTotal.group(1)  == "running" :
				nRunning += int(mTotal.group(2))
			elif mTotal.group(1)  == "idle" :
				nIdle += int(mTotal.group(2))
			elif mTotal.group(1)  == "unsubmitted" :
				nUnSub += int(mTotal.group(2))
			continue	
			
	nOther = nTotal - nFinished -nRunning -nIdle-nUnSub-nFailed 	
	pS = '{m: <{w}}'.format(m=name,w=maxSize)
	pS += '{m: <{w}}'.format(m=nTotal,w=10)
	
	if nFinished :
		if(nFinished == nTotal) :
			pS += "\033[36mAll done!\033[0m"
			print(pS)
			continue
		pT = "{}/{:.0f}%".format(nFinished,100.0*nFinished/nTotal)
		pS += "\033[36m{m: <{w}}\033[0m".format(m=pT,w=10)
	
	else :
		pS += '\033[36m{m: <{w}}\033[0m'.format(m="-",w=10)		
	if nRunning :	
		pT = "{}/{:.0f}%".format(nRunning,100.0*nRunning/nTotal)
		pS += "\033[93m{m: <{w}}\033[0m".format(m=pT,w=10)
	else :
		pS += "\033[93m{m: <{w}}\033[0m".format(m="-",w=10)	
	if nFailed :
		pT = "{}/{:.0f}%".format(nFailed,100.0*nFailed/nTotal)
		pS += "\033[91m{m: <{w}}\033[0m".format(m=pT,w=10)
	else :
		pS += "\033[91m{m: <{w}}\033[0m".format(m="-",w=10)	
	if nIdle :
		pT = "{}/{:.0f}%".format(nIdle,100.0*nIdle/nTotal)
		pS += "{m: <{w}}".format(m=pT,w=10)
	else :
		pS += "{m: <{w}}".format(m="-",w=10)
	if nUnSub :
		pT = "{}/{:.0f}%".format(nUnSub,100.0*nUnSub/nTotal)
		pS += "{m: <{w}}".format(m=pT,w=10)
	else :
		pS += "{m: <{w}}".format(m="-",w=10)
	if nOther :
		pT = "{}/{:.0f}%".format(nOther,100.0*nOther/nTotal)
		pS += "{m: <{w}}".format(m=pT,w=10)
	else :
		pS += "{m: <{w}}".format(m="-",w=10)
		
		
	
	print(pS)	
		
	
	