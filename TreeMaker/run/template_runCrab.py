from CRABClient.UserUtilities import config
config = config()

config.General.requestName = '_requestName_'
config.General.workArea = '_workarea_'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '_psetName_'

config.Data.inputDataset = '_inputDataset_'
_CONFIGFILES_
config.JobType.inputFiles = []
                             
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = _nFilesPerJob_ 
config.Data.outLFNDirBase = '_outLFNDirBase_'
config.Data.publication = False

config.Site.storageSite = 'T3_US_FNALLPC'
