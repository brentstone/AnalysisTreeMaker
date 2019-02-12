#!/usr/bin/python
import os
import sys
import re
import argparse
import subprocess

def getMatch(summary, entry) :
	match = re.match( (".*\"%s\":([0-9]+)[^0-9]+.*" % entry),summary)
	if not match :
		return "?"
	else:
		return match.group(1)


parser = argparse.ArgumentParser(description='Get DAS summary info')
parser.add_argument("-i", "--dasEntry", dest="dasEntry", default="", help="Input DAS string")
parser.add_argument("-s", "--getAllStatus",  dest="getAllStatus", action='store_true', default=False, help="Should we look for all samples? [Default: False]")

args = parser.parse_args()

if args.getAllStatus :
	dascmd =  ("dasgoclient --query=\"dataset status=* dataset=%s\"" % args.dasEntry)
else :
	dascmd =  ("dasgoclient --query=\"dataset=%s\"" % args.dasEntry)
#dascmd = "cat /Users/nmccoll/Dropbox/Work/Projects/HHbbWW/11_14_18_legacyFrmwrk/test.txt"
#print dascmd
dasResult = subprocess.Popen(dascmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
print "Short name \t Size \t Num. of files \t Num. of events \t Full DAS \t Parent"
for line in dasResult[0].splitlines():
#	dascmd2 = "cat /Users/nmccoll/Dropbox/Work/Projects/HHbbWW/11_14_18_legacyFrmwrk/test2.txt" 
	dascmd2 = ("dasgoclient --query=\"summary dataset=%s\"" % line) 
	dasResult2 = subprocess.Popen(dascmd2, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
	summary = dasResult2[0].rstrip('\n')
	
	dascmd3 = ("dasgoclient --query=\"parent dataset=%s\"" % line) 
	dasResult3 = subprocess.Popen(dascmd3, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
	parent = dasResult3[0].rstrip('\n')
	
	file_size = getMatch(summary,"file_size")
	nblocks = getMatch(summary,"nblocks")
	nevents = getMatch(summary,"nevents")
	nfiles = getMatch(summary,"nfiles")
	nlumis = getMatch(summary,"nlumis")
	num_block = getMatch(summary,"num_block")
	num_event = getMatch(summary,"num_event")
	num_file = getMatch(summary,"num_file")
	num_lumi = getMatch(summary,"num_lumi")
	dasShortMatch = re.match("^/(.+)/.+/.+$",line)
	if not dasShortMatch:
		dasShort = "?"
	else :
		dasShort = dasShortMatch.group(1)
	print dasShort + "\t" +file_size+ "\t"+ num_file + "\t"+ num_event  +"\t" +line+"\t" +parent


