#!/usr/bin/python
import os
import sys
import re
import argparse
import subprocess

parser = argparse.ArgumentParser(description='Prepare and submit ntupling jobs')
parser.add_argument("-c", "--config", dest="config", default="runTestAnalyzer.py", help="Configuration file to be run using cmsRun to run. [Default: runTestAnalyzer.py]")
parser.add_argument("-i", "--inputData", dest="inputData", default="datasets.conf", help="Input dataset. [Default: datasets.conf]")
parser.add_argument("-j", "--jobdir", dest="jobdir", default="jobs", help="Directory for job files. [Default: jobs]")
parser.add_argument("-o", "--outdir", dest="outdir", default="/eos/uscms/store/user/${USER}/13TeV/processed", help="Output directory for ntuples. [Default: \"/eos/uscms/store/user/${USER}/13TeV/ntuples\"]")
parser.add_argument("-t", "--crabTemp", dest="crabTemp", default="template_makeCrabJob.py", help="crab template file. [Default: \"template_makeCrabJob.py\"]")
args = parser.parse_args()


def addSample(name,sample,datarun,cross,numE,numF,cmdLine,dasName,configs) :
	crab_template_file = args.crabTemp
	with open(crab_template_file, 'r') as crfile:
		crabconfig = crfile.read()
	outputFile = name + ".root"
	configParams = "config.JobType.pyCfgParams = ["
	if cmdLine != '-' :
		configParams += ("%s" % cmdLine   )
	configParams += ("\'sample=%s\',\'dataRun=%s\',\'isCrab=True\','outputFile=%s\'" % (sample, "NONE" if datarun == "-" else datarun,outputFile))  
	configParams += "]"
		
	replace_dict = {'_requestName_':name,
				'_workarea_':args.jobdir,
				'_psetName_':args.config,
				'_inputDataset_':dasName,
				'_CONFIGFILES_':configParams,
				'_nFilesPerJob_':str(numF),
				'_outLFNDirBase_':re.sub(r'/eos/[a-z]+/store', '/store', args.outdir)}
	for key in replace_dict:
		crabconfig = crabconfig.replace(key, replace_dict[key])
	jsonFile = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt"
	runRange = '271000-290000'
	if datarun != "-" :
		crabconfig += ("\nconfig.Data.lumiMask = \'%s\'\n" % jsonFile)
		crabconfig += ("\nconfig.Data.runRange = \'%s\'\n" % runRange)
	cfgfilename = ("%s/crab_submit_%s.py" % (args.jobdir, name))
	configs.append(cfgfilename)
	with open(cfgfilename, 'w') as cfgfile:
		cfgfile.write(crabconfig)
	print cfgfilename


os.system("mkdir -p %s" % args.jobdir)
if args.outdir.startswith("/eos/cms/store/user") or args.outdir.startswith("/store/user") :
	os.system("%s mkdir -p %s" % (eos, args.outdir))
else :
	os.system("mkdir -p %s" % args.outdir)

crab_configs = []
inputData = open(args.inputData, "r")
for line in inputData:
	if re.match("\s*#.*", line) : 
		continue
	match = re.match("^\s*(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s*$", line)
	if not match : 
		print "Do not understand:"
		print line
		continue
	addSample(match.group(1),match.group(2),match.group(3),match.group(4),
			match.group(5),match.group(6),match.group(7),match.group(8),crab_configs)
print "Creating submission file: submitall.sh"
with open("submitall.sh", "w") as script:
	script.write("#!/bin/bash\n\n")
	for cfg in crab_configs:
		script.write("crab submit -c %s\necho\n\n" % cfg)

os.system("chmod +x submitall.sh")
print "Done!"
exit()
