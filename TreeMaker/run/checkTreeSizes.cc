{
  float total = 0;
  std::vector<TString> col = {"event","genParticle","ak4Jet","ak8PuppiNoLepJet","ak8PuppiJet","electron","muon"};
  std::vector<TRegexp> colR;
  std::vector<float> colSize(col.size(),0);
  for(unsigned int iC = 0; iC < col.size(); ++iC){
    colR.emplace_back("*"+col[iC]+"*",kTRUE);
  }
  
  std::vector<TString> brNames;
  std::vector<float> brZS;
  std::vector<float> brTS;
  std::vector<int> brIs;
          
  treeMaker->cd();
  TBranch *br = 0;
  TIter next(const_cast<TTree*>(Events)->GetListOfBranches());
  TBranch::ResetCount();
  while ((br= (TBranch*)next())) {
     TString st = br->GetName();
     st.ReplaceAll("/","_");
     // std::cout << st <<"\t"<< br->GetTotalSize()<<"\t"<<br->GetZipBytes()<<"\t" <<(br->GetTotBytes() > 0 ? float(br->GetZipBytes())/float(br->GetTotBytes()) : float(br->GetTotBytes())) <<std::endl;
     int brI =-1;
     for(unsigned int iC = 0; iC < col.size(); ++iC){
       if(st.Index(colR[iC])!= kNPOS) brI = iC;
     }
     brNames.push_back(st);
     brZS.push_back(br->GetZipBytes());
     brTS.push_back(br->GetTotalSize());
     brIs.push_back(brI);
     total+= br->GetZipBytes();
     if(brI>=0) colSize[brI] += br->GetZipBytes();
     
     
  }
  float nEntries = Events->GetEntries();
  std::cout <<"Category\tDisk size\tPer event\tOut of total(%)\n";
  std::cout <<"Total\t"<< total<<"\t"<< total/nEntries <<"\t-"<<std::endl;  
  for(unsigned int iC = 0; iC < col.size(); ++iC){
    std::cout << col[iC]<<"\t"<<colSize[iC]<<"\t"<<colSize[iC]/nEntries<<"\t"<<TString::Format("%.1f",100.*colSize[iC]/total)<<std::endl;
  }
  std::cout <<"Branch\tTotal size\tDisk size\tOut of category(%)\tOut of all(%)\n";
  for(unsigned int iB = 0; iB < brNames.size(); ++iB){
    std::cout << brNames[iB]<<"\t"<<brTS[iB]<<"\t"<<brZS[iB] <<"\t"<<TString::Format("%.1f",100.*( brIs[iB]>= 0 ? brZS[iB]/colSize[brIs[iB]] : 0.0))<<"\t"<<TString::Format("%.1f",100.*brZS[iB]/total) <<std::endl;    
  }
      
}