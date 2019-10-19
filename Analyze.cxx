//  Created by Юрий Бордулев on 28/01/2019.
#include "Analyze.hpp"
//Declarations of functions protoypes
//-----------------------------------------------------------------------------------
//Function, that converts UNIX time to ddmmyyyy
void convert_to_date(char* outStr, UInt_t unixTime, int formattype); //FormatType 1 = ddmmyyy, FormatType 2 = dd/mm/yyy
// Function, that corrects the VMMIds which are measured second and third time and look like 100xxx or 10xxxx
int correct_VMMId(int number, uint datetime);
//Function that Fills Flag(write/not write)
// Flag 0 - not to write in DB
// Flag 1 - write in DB
// Flag 2 - several measuremets with same best statuses
//      21 - the number of dead channels is minimum here -> write in DB
//      20 - the number of dead channels is not a minimum here -> not to write in DB
//      22 - the number of dead channels is minimum but equal in several measurements
//          221 - the measurement is more recent -> write in DB
//          220 - the measurement is less recent -> don't write in DB
//----------------------------------------------------------------------------------------------
void fillFlags(int arr[][11], int arrSize, int ChipStatus_col, int n_flag_col, int n_dead_ch_col, int neg_or_pos);
//Function that calculates the Total status for the chip, which have several tests
int calculate_Total_Stat(int arr[][11], int arrSize, int VMMID);
//Function that calculates General Status for both polarities (Perfect/PerfectNEG/PerfectPos/Passed/Failed)
int calculate_Total_Stat_two_Polarities(int arr[][11], int arrSize, int VMMID); 
//Function that fiils the Database.txt file
void write_to_Database(int arr[][11], int arrSize, int neg_or_pos); //neg = 0, pos = 1
//Function that fills the Certificate file
void write_to_Certificate(int arr[][11], int arrSize, int neg_or_pos, int FR_KR); //neg = 0, pos = 1	FR=0, KR=1
//Function that fiils the Status file
void write_to_Status(int arr[][11], int arrSize, int FR_KR); //neg = 0, pos = 1, FR=0, KR=1
//Function that fills the Histogram
void Fill_Hist(int arr[][11], int arrSize, int FR_KR); //FR = 0, KR = 1
//Function that print header in Status and Cerificate file
void write_header();
//Function that checks if there is a file
int checkFile(int neg_or_pos);
//Draw one 1D hist in a a page
void one_1Dhist(string xname, string yname, TH1F *hist, int log_or_not);
//Draw two 1D hist in a a page (log scale)
void two_1Dhist(string xname1, string yname1, string xname2, string yname2, TH1F *hist1, TH1F *hist2); 
//Draw one 2D hist in a a page
void one_2Dhist(string xname, string yname, TH2F *hist);
//Draw two 2D hist in a a page
void two_2Dhist(string xname1, string yname1, string xname2, string yname2, TH2F *hist1, TH2F *hist2);
//Calculate the number of dead channels
int Number_of_dead_channel();
//Function that prepares the file to futher download dead channels to Electronics DB "SN - dead сhannels neg"
int write_to_Global(int arr[][11], int arrSize);
//Function that pront the value in a log file, or (if there is no value) prints "-"
int print_to_log_to_Global(float value, ofstream& file);
//---------------------------------------- Main code ---------------------------------------------
int main()
{
    
    rootfile = new TFile(path_to_root); //Открываем файл root
    roottree = (TTree*)rootfile->Get("ChipConstants"); //Из root файла достаем дерево ChipConstants
    Int_t nentries = (Int_t)roottree->GetEntries(); //Общее количество событий
    
    if (!roottree) {
        cout<<"ChipConstants not found"<<endl;
        return 0;
    } // roottree is correct?
    pos_DEAD_CHANNELS = 0;
    neg_DEAD_CHANNELS = 0;
    numberBadCh_ARTneg = 0;
    numberBadCh_ARTpos = 0;
    
    roottree->SetBranchAddress("userID",&userID);
    roottree->SetBranchAddress("partID",&partID);
    roottree->SetBranchAddress("FPGA_VER", &FPGA_VER);
    roottree->SetBranchAddress("FAILED",&FAILED);
    roottree->SetBranchAddress("VMMId",&VMMId);
    roottree->SetBranchAddress("boardId",&boardId);
    roottree->SetBranchAddress("chipStatusNEG",&chipStatusNEG);
    roottree->SetBranchAddress("chipStatusPOS",&chipStatusPOS);
    roottree->SetBranchAddress("chipStatus",&chipStatus);
    roottree->SetBranchAddress("pos_DEAD_CHANNELS",&pos_DEAD_CHANNELS, &b_pos_DEAD_CHANNELS);
    roottree->SetBranchAddress("neg_DEAD_CHANNELS",&neg_DEAD_CHANNELS, &b_neg_DEAD_CHANNELS);

    roottree->SetBranchAddress("numberBadCh_ARTneg",&numberBadCh_ARTneg);
    roottree->SetBranchAddress("numberBadCh_ARTpos",&numberBadCh_ARTpos);
    roottree->SetBranchAddress("countBadCh_ARTneg", &countBadCh_ARTneg);
    roottree->SetBranchAddress("EntriesARTneg", &EntriesARTneg);
    roottree->SetBranchAddress("countBadCh_ARTpos", &countBadCh_ARTpos);
    roottree->SetBranchAddress("EntriesARTpos", &EntriesARTpos);
    roottree->SetBranchAddress("dateTime",&dateTime);
    roottree->SetBranchAddress("folderTesting",&folderTesting);

    roottree->SetBranchAddress("bLinePos",&bLinePos);
    roottree->SetBranchAddress("bLine",&bLineNeg);
    roottree->SetBranchAddress("bLinePausePos",&bLinePausePos);
    roottree->SetBranchAddress("bLine_RMS", bLine_RMS);
    
    roottree->SetBranchAddress("PDOMEAN_L0",&PDOMEAN_L0NEG);
    roottree->SetBranchAddress("PDOMEAN_L0POS",&PDOMEAN_L0POS);
    roottree->SetBranchAddress("TDOMEAN_L0POS",&TDOMEAN_L0POS);
    roottree->SetBranchAddress("TDOMEAN_L0",&TDOMEAN_L0NEG);    
    roottree->SetBranchAddress("PDOEntriesMEAN_L0POS",&PDOEntriesMEAN_L0POS);
    roottree->SetBranchAddress("PDOEntriesMEAN_L0",&PDOEntriesMEAN_L0NEG);
    roottree->SetBranchAddress("TDOEntriesMEAN_L0POS",&TDOEntriesMEAN_L0POS);
    roottree->SetBranchAddress("TDOEntriesMEAN_L0",&TDOEntriesMEAN_L0NEG);
    roottree->SetBranchAddress("EntriesARTneg",&EntriesARTneg);
    roottree->SetBranchAddress("EntriesARTpos",&EntriesARTpos);
    roottree->SetBranchAddress("PDORMS_L0", PDORMS_L0);
    roottree->SetBranchAddress("PDORMS_L0POS", PDORMS_L0POS);
    roottree->SetBranchAddress("PDOIntercept_L0", PDOIntercept_L0);//45
    roottree->SetBranchAddress("PDOIntercept_L0POS", PDOIntercept_L0POS);//46
    roottree->SetBranchAddress("PDOSlope_L0", PDOSlope_L0);//47
    roottree->SetBranchAddress("PDOSlope_L0POS", PDOSlope_L0POS);//48
    roottree->SetBranchAddress("TDORMS_L0",TDORMS_L0);//49
    roottree->SetBranchAddress("TDORMS_L0POS",TDORMS_L0POS);//50
    roottree->SetBranchAddress("TDOIntercept_L0", TDOIntercept_L0);//51
    roottree->SetBranchAddress("TDOIntercept_L0POS", TDOIntercept_L0POS);//52
    roottree->SetBranchAddress("TDOSlope_L0", TDOSlope_L0);//53
    roottree->SetBranchAddress("TDOSlope_L0POS", TDOSlope_L0POS);//54
    roottree->SetBranchAddress("MEAN_DDOneg",&MEAN_DDOneg);
    roottree->SetBranchAddress("MEAN_DDOpos",&MEAN_DDOpos);
    
    roottree->SetBranchAddress("chipStatus",&chipStatus);

    roottree->SetBranchAddress("VDDP_A_Max_L0",&VDDP_A_Max_L0, &b_VDDP_A_Max_L0);
    roottree->SetBranchAddress("VDDP_A_Max_L0POS",&VDDP_A_Max_L0POS);
    roottree->SetBranchAddress("VDD_A_Max_L0",&VDD_A_Max_L0);
    roottree->SetBranchAddress("VDD_A_Max_L0POS",&VDD_A_Max_L0POS);
    roottree->SetBranchAddress("VDDD_A_Max_L0",&VDDD_A_Max_L0);
    roottree->SetBranchAddress("VDDD_A_Max_L0POS",&VDDD_A_Max_L0POS);
    roottree->SetBranchAddress("VDAD_A_Max_L0",&VDAD_A_Max_L0);
    roottree->SetBranchAddress("VDAD_A_Max_L0POS",&VDAD_A_Max_L0POS);

    roottree->SetBranchAddress("tempMax_L0", &tempMax_L0);
    roottree->SetBranchAddress("tempMax_L0POS", &tempMax_L0POS);

    // Fill the array, that contain the table
    // 0 VMMId --- 1 Number of entries with this ID --- 2 ChipStatus NEG --- 3 ChipStatus POS --- 4 FlagNEG (Write or don't write in DB) --- 5 FlagPOS --- 6 neg_DEAD_Ch --- 7 pos_DEAD_ch --- 8 DateTime, when the measurement was done --- 9 General Status
    // 10 - boardId
    Int_t VMM_array[nentries][11];
    //Fill the Array, with VMMID [i][0] and chip status NEG [i][2] and POS and [i][3]
    for (i=0; i<nentries; i++) {
        roottree->GetEntry(i);
        VMM_array[i][0] = correct_VMMId(VMMId, dateTime); //Fill with VMMIds
        VMM_array[i][2] = chipStatusNEG;   //Fill with ChipStatuses
        VMM_array[i][3] = chipStatusPOS;
        size_neg = neg_DEAD_CHANNELS->size();
        size_pos = pos_DEAD_CHANNELS->size();
        VMM_array[i][6] = size_neg;  // NEG
        VMM_array[i][7] = size_pos; //POS
        VMM_array[i][8] = dateTime; //Datetime of the measurement (entry)
        VMM_array[i][10] = boardId;
        if (chipStatusPOS == 2 && chipStatusNEG == 2){
            VMM_array[i][9] = 1000000;
        }
        if (chipStatusPOS == 1 && chipStatusNEG == 1){
            VMM_array[i][9] = 100000;
        }
        if (chipStatusPOS == 2 && chipStatusNEG == 1){
            VMM_array[i][9] = 10000;
        }
        if (chipStatusPOS == 1 && chipStatusNEG == 2){
            VMM_array[i][9] = 1000;
        }
        if (chipStatusPOS != 0 && chipStatusNEG == 0){
            VMM_array[i][9] = 100;
        }
        if (chipStatusPOS == 0 && chipStatusNEG != 0){
            VMM_array[i][9] = 10;
        }
        if (chipStatusPOS == 0 && chipStatusNEG == 0){
            VMM_array[i][9] = 1;
        }
    }
    //Fill the Array, with number of entries for each chip [i][2]
    for (i = 0; i<nentries; i++){
        counter = 1;
        for (j = 0; j<nentries; j++){ //Searching again in this array - if this chip was measured again
            if (i != j && VMM_array[i][0] == VMM_array[j][0])
                counter++;
        }
        VMM_array[i][1] = counter;
    }
    
    fillFlags(VMM_array, nentries, 2, 4, 6, 0);    //Fill Flag(write/not write) for all Neg measurements
    fillFlags(VMM_array, nentries, 3, 5, 7, 1);    //Fill Flag(write/not write) for all Pos measurements
    
    //Write to file Database.txt
    /*fileo.open(path_to_file_database);
    fileo<<"**********************************************************"<<endl;
    counter = 0;
    counter1 = 0;
    counter2 = 0;
    write_to_Database(VMM_array, nentries, 0); //BEST NEG MEASUREMENTS
    write_to_Database(VMM_array, nentries, 1); //BEST POS MEASUREMENTS
    fileo.close();
    //cout<<nentries<<" "<<counter1<<" "<<counter2<<endl;
    //Write to file CERT_FR3018_FR3118.txt
    // Write the header
    fileo.open(path_to_file_certificate_FR);
    write_header();
    //Write data about best measurements in certificate FR PartNumber
    write_to_Certificate(VMM_array, nentries, 0, 0); //BEST NEG MEASUREMENTS
    write_to_Certificate(VMM_array, nentries, 1, 0); //BEST POS MEASUREMENTS
    fileo.close();

    fileo.open(path_to_file_certificate_KR);
    write_header();
    //Write data about best measurements in certificate KR PartNumber
    write_to_Certificate(VMM_array, nentries, 0, 1); //BEST NEG MEASUREMENTS
    write_to_Certificate(VMM_array, nentries, 1, 1); //BEST POS MEASUREMENTS
    fileo.close();
    
    //Write to file STATUS_FR3018_FR3118.txt
    fileo.open(path_to_file_status_FR);
    write_header();
    write_to_Status(VMM_array, nentries, 0);
    fileo.close();

    fileo.open(path_to_file_status_KR);
    write_header();
    write_to_Status(VMM_array, nentries, 1);
    fileo.close();
    
    
    Fill_Hist(VMM_array, nentries, 0);
    Fill_Hist(VMM_array, nentries, 1); 
    */ 
    fileo.open(path_to_file_GlobalLog);//General information for database
    fileo_dead_ch.open(path_to_file_GlobalLog_dead_ch);//Dead channels information
    fileo_dead_ch_Bline.open(path_to_file_GlobalLog_dead_ch_Bline);//Dead channels Bline
    fileo_dead_ch_ART.open(path_to_file_GlobalLog_dead_ch_ART);//Dead channels ART
    fileo_dead_ch_PDO.open(path_to_file_GlobalLog_dead_ch_PDO);//Dead channels PDO
    fileo_dead_ch_TDO.open(path_to_file_GlobalLog_dead_ch_TDO);//Dead channels TDO
    fileo_dead_ch_DDO.open(path_to_file_GlobalLog_dead_ch_DDO);//Dead channels DDO*/
    fileo_Bline.open(path_to_file_GlobalLog_Bline);//Dead channels Bline   
    fileo_PDO_mean.open(path_to_file_GlobalLog_PDO_mean);
    fileo_TDO_mean.open(path_to_file_GlobalLog_TDO_mean);
    fileo_PDO_entries.open(path_to_file_GlobalLog_PDO_entries);
    fileo_TDO_entries.open(path_to_file_GlobalLog_TDO_entries);
    fileo_DDO_mean.open(path_to_file_GlobalLog_DDO_mean);
    fileo_ART_entries.open(path_to_file_GlobalLog_ART_entries);

    write_to_Global(VMM_array, nentries);
    
    fileo.close();
    fileo_dead_ch.close();
    fileo_dead_ch_Bline.close();
    fileo_dead_ch_ART.close();
    fileo_dead_ch_PDO.close();
    fileo_dead_ch_TDO.close();
    fileo_dead_ch_DDO.close();
    fileo_Bline.close();
    fileo_PDO_mean.close();
    fileo_TDO_mean.close();
    fileo_PDO_entries.close();
    fileo_TDO_entries.close();
    fileo_ART_entries.close();
    fileo_DDO_mean.close();

    return 0;
}
//--------------------------------------------------------------------------------------------------------------------------
//Functions Definition
void Fill_Hist(int arr[][11], int arrSize, int FR_KR){ //neg = 0, pos = 1
    //Fill pdf with hist
    //gStyle->SetOptStat("e"); //Entry, ..., .... Use Google
    //gStyle->SetPalette(kTemperatureMap); //Choose the Colour palette
    TString path_to_hist;
    int startdate, stopdate;
    if (FR_KR == 0){
        path_to_hist = path_to_hist_FR;
        startdate = 1462060800; //1 May 2016
        stopdate = 1556668800; //1 May 2019
    }
    else{
        path_to_hist = path_to_hist_KR;
        //startdate = 1556668800; //1 May 2019
        startdate = 1557784800; //14 May
        stopdate = 1651363200; //1 May 2022
    }
    Canv = new TCanvas();
    sprintf(pdfname,path_to_hist);
    sprintf(pdfopen, "%s[", pdfname);
    sprintf(pdfclose, "%s]", pdfname);
    Canv->Print(pdfopen, "pdf"); //second parameter is a format
    
    hSatus_VS_VMMId->Reset("ICESM");
    hbline->Reset("ICESM");
    hblineVMM->Reset("ICESM");
    hblineTEMP->Reset("ICESM");
    hBlinecor->Reset("ICESM");
    hALLCHANNEL->Reset("ICESM");
    hPDOEntriesMEANL0->Reset("ICESM");
    hALLCHANNELtdol0->Reset("ICESM");
    hTDOEntriesMEANL0->Reset("ICESM");
    hALLCHANNELpdol0pos->Reset("ICESM");
    hPDOEntriesMEANL0POS->Reset("ICESM");
    hALLCHANNELtdol0pos->Reset("ICESM");
    hTDOEntriesMEANL0POS->Reset("ICESM");
    hDDOnegneg_ch->Reset("ICESM");
    hDDOpos_ch->Reset("ICESM");
    hVDD->Reset("ICESM");
    hVDDpos->Reset("ICESM");
    hVDDP->Reset("ICESM");
    hVDDPpos->Reset("ICESM");
    hVDDD->Reset("ICESM");
    hVDDDpos->Reset("ICESM");
    hVDAD->Reset("ICESM");
    hVDADpos->Reset("ICESM");
    htempAVDD->Reset("ICESM");
    htempAVDDpos->Reset("ICESM");
    htempAVDDP->Reset("ICESM");
    htempAVDDPpos->Reset("ICESM");
    htempAVDDD->Reset("ICESM");
    htempAVDDDpos->Reset("ICESM");
    htempA->Reset("ICESM");
    htempApos->Reset("ICESM");
    hsumcurrent_L0->Reset("ICESM");
    hsumcurrent_L0POS->Reset("ICESM");
    hARTnegNEG->Reset("ICESM");
    hEntriesARTneg->Reset("ICESM");
    hARTnegNEG1->Reset("ICESM");
    hARTcountBad->Reset("ICESM");
    hARTpos->Reset("ICESM");
    hEntriesARTpos->Reset("ICESM");
    hARTpos2->Reset("ICESM");
    hARTposcountBad->Reset("ICESM");
    hPDORMS_L0->Reset("ICESM");
    hPDORMS_L0POS->Reset("ICESM");
    hblineRMS->Reset("ICESM");
    hPDOIntercept_L0->Reset("ICESM");
    hPDOIntercept_L0POS->Reset("ICESM");
    hPDOSlope_L0->Reset("ICESM");
    hPDOSlope_L0POS->Reset("ICESM");
    hTDORMS_L0->Reset("ICESM");
    hTDORMS_L0POS->Reset("ICESM");
    hTDOIntercept_L0->Reset("ICESM");
    hTDOIntercept_L0POS->Reset("ICESM");
    hTDOSlope_L0->Reset("ICESM");
    hTDOSlope_L0POS->Reset("ICESM");
 
    for (i = 0; i<arrSize; i++){
        roottree->GetEntry(i);
        if(dateTime>startdate && dateTime<stopdate){ //Do not include chips, that were measured before 19 July 2018 in the hists 
            if (arr[i][4] == 1 || arr[i][4] == 21 || arr[i][4] == 221){ //we will write only the best measurements for NEG measurments 
                for (k = 0; k<64; k++){
                    hbline->Fill(bLineNeg[k]); //fill hist for Baseline
                    hblineVMM->Fill(bLineNeg[k], VMMId); //3rd hist
                    hblineTEMP->Fill(bLineNeg[k],tempMax_L0); //4th
                    hblineRMS->Fill(bLine_RMS[k]);//44
                    if (tempMax_L0>0){
                        if (chipStatus>0 && bLinePausePos == 2)
                            hBlinecor->Fill(bLineNeg[k],bLinePos[k]); //5th
                        hALLCHANNEL->Fill(PDOMEAN_L0NEG[k], k);//6th
                        hPDOEntriesMEANL0->Fill(PDOEntriesMEAN_L0NEG[k],k);//7
                        hALLCHANNELtdol0->Fill(TDOMEAN_L0NEG[k],k);//8
                        hTDOEntriesMEANL0->Fill(TDOEntriesMEAN_L0NEG[k],k);//!9
                        hALLCHANNELpdol0pos->Fill(PDOMEAN_L0POS[k],k);//10
                        hPDOEntriesMEANL0POS->Fill(PDOEntriesMEAN_L0POS[k],k);//11
                        hALLCHANNELtdol0pos->Fill(TDOMEAN_L0POS[k],k);//12
                        hTDOEntriesMEANL0POS->Fill(TDOEntriesMEAN_L0POS[k],k);//13
                        hDDOnegneg_ch->Fill(MEAN_DDOneg[k],k);//14
                        hEntriesARTneg->Fill(EntriesARTneg[k],k);//35
                        hPDORMS_L0->Fill(PDORMS_L0[k]);//42
                        hPDOIntercept_L0->Fill(PDOIntercept_L0[k]);//45
                        hPDOSlope_L0->Fill(PDOSlope_L0[k]);//47
                        hTDORMS_L0->Fill(TDORMS_L0[k]);//49
                        hTDOIntercept_L0->Fill(TDOIntercept_L0[k]);//51
                        hTDOSlope_L0->Fill(TDOSlope_L0[k]);//53
                    }
                }
                if (tempMax_L0>0){
                    hVDD->Fill(VDD_A_Max_L0); //16
                    hVDDP->Fill(VDDP_A_Max_L0);//18
                    hVDDD->Fill(VDDD_A_Max_L0);//20
                    hVDAD->Fill(VDAD_A_Max_L0);//22
                    htempAVDD->Fill(tempMax_L0,VDD_A_Max_L0);//24
                    htempAVDDP->Fill(tempMax_L0,VDDP_A_Max_L0);//26
                    htempAVDDD->Fill(tempMax_L0,VDDD_A_Max_L0);//28
                    htempA->Fill(tempMax_L0,VDAD_A_Max_L0);//30
                    hsumcurrent_L0->Fill((VDDP_A_Max_L0 + VDDD_A_Max_L0 + VDAD_A_Max_L0 + VDD_A_Max_L0));//32
                    hARTnegNEG->Fill(VMMId,countBadCh_ARTneg);//34
                    if (countBadCh_ARTneg>0 ){
                        for (k=0; k<countBadCh_ARTneg;k++){
                            hARTnegNEG1->Fill(VMMId, numberBadCh_ARTneg->at(k));//36
                            hARTcountBad->Fill(numberBadCh_ARTneg->at(k));//37
                        }
                    }
                }
                hSatus_VS_VMMId->Fill(VMMId, chipStatus); //0th
            }
            if (arr[i][5] == 1 || arr[i][5] == 21 || arr[i][5] == 221){ //we will write only the best measurements for POS measurments
                for (k = 0; k<64; k++){
                    if (tempMax_L0POS>0){
                        hDDOpos_ch->Fill(MEAN_DDOpos[k], k);//15
                        hEntriesARTpos->Fill(EntriesARTpos[k],k);//39
                        hPDORMS_L0POS->Fill(PDORMS_L0POS[k]);//43
                        hPDOIntercept_L0POS->Fill(PDOIntercept_L0POS[k]);//46
                        hPDOSlope_L0POS->Fill(PDOSlope_L0POS[k]);//48
                        hTDORMS_L0POS->Fill(TDORMS_L0POS[k]);//50 
                        hTDOIntercept_L0POS->Fill(TDOIntercept_L0POS[k]);//52
                        hTDOSlope_L0POS->Fill(TDOSlope_L0POS[k]);//54
                    }
                }
                if (tempMax_L0POS>0){
                    hVDDpos->Fill(VDD_A_Max_L0POS);//17
                    hVDDPpos->Fill(VDDP_A_Max_L0POS);//19
                    hVDDDpos->Fill(VDDD_A_Max_L0POS);//21
                    hVDADpos->Fill(VDAD_A_Max_L0POS);//23
                    htempAVDDpos->Fill(tempMax_L0POS,VDD_A_Max_L0POS);//25
                    htempAVDDPpos->Fill(tempMax_L0POS,VDDP_A_Max_L0POS);//27
                    htempAVDDDpos->Fill(tempMax_L0POS,VDDD_A_Max_L0POS);//29
                    htempApos->Fill(tempMax_L0POS,VDAD_A_Max_L0POS);//31
                    hsumcurrent_L0POS->Fill((VDDP_A_Max_L0POS + VDDD_A_Max_L0POS + VDAD_A_Max_L0POS + VDD_A_Max_L0POS));//32
                    hARTpos->Fill(VMMId,countBadCh_ARTpos);//38
                    if (countBadCh_ARTpos>0){
                        for (k=0; k<countBadCh_ARTpos;k++){
                            hARTpos2->Fill(VMMId, numberBadCh_ARTpos->at(k));//40
                            hARTposcountBad->Fill(numberBadCh_ARTpos->at(k));//41
                        }
                    }
                }
            }
        }
    }
    //Print the Baseline in log scale
    gStyle->SetOptStat("eou");
    one_2Dhist("VMMId", "Status", hSatus_VS_VMMId);//00
    one_1Dhist("mV", "count channels", hbline, 1);//01
    one_1Dhist("mV", "count channels", hbline, 0);//02
    one_2Dhist("VMMId", "Baseline, mV", hblineVMM);//03
    one_2Dhist("Baseline, mV", "Temp, C", hblineTEMP);//04
    one_2Dhist("baseline NEG, mV", "baseline POS, mV", hBlinecor);//05
    one_2Dhist("ADC", "channel", hALLCHANNEL);//06
    one_2Dhist("Entries", "channel", hPDOEntriesMEANL0);//07
    one_2Dhist("ADC", "channel", hALLCHANNELtdol0);//08
    one_2Dhist("Entries", "channel", hTDOEntriesMEANL0);//09
    one_2Dhist("ADC", "channel", hALLCHANNELpdol0pos);//10
    one_2Dhist("Entries", "channel", hPDOEntriesMEANL0POS);//11
    one_2Dhist("ADC", "channel", hALLCHANNELtdol0pos);//12
    one_2Dhist("Entries", "channel", hTDOEntriesMEANL0POS);//13
    one_2Dhist("ADC", "channel", hDDOnegneg_ch);//14
    one_2Dhist("ADC", "channel", hDDOpos_ch);//15
    two_1Dhist("Current, A", "count VMMId", "Current, A", "count VMMId", hVDD, hVDDpos);//16 and 17  
    two_1Dhist("Current, A", "count VMMId", "Current, A", "count VMMId", hVDDP, hVDDPpos);//18 and 19
    two_1Dhist("Current, A", "count VMMId", "Current, A", "count VMMId", hVDDD, hVDDDpos);//20 and 21
    two_1Dhist("Current, A", "count VMMId", "Current, A", "count VMMId", hVDAD, hVDADpos);//22 and 23
    two_2Dhist("temp, C", "current", "temp, C", "current", htempAVDD, htempAVDDpos);//24 and 25
    two_2Dhist("temp, C", "current", "temp, C", "current", htempAVDDP, htempAVDDPpos);//26 and 27
    two_2Dhist("temp, C", "current", "temp, C", "current", htempAVDDD, htempAVDDDpos);//28 and 29
    two_2Dhist("temp, C", "current", "temp, C", "current", htempA, htempApos);//30 and 31
    two_1Dhist("Current, A", "count VMMId", "Current, A", "count VMMId", hsumcurrent_L0, hsumcurrent_L0POS);//32 and 33
    one_2Dhist("VMMId", "count channels", hARTnegNEG);//34
    one_2Dhist("Entries", "count channels", hEntriesARTneg);//35
    one_2Dhist("VMMId", "channel", hARTnegNEG1);//36
    one_1Dhist("count bad ART", "channel", hARTcountBad, 0);//37
    one_2Dhist("VMMId", "count channels", hARTpos);//38
    one_2Dhist("Entries", "count channels", hEntriesARTpos);//39
    one_2Dhist("VMMId", "channel", hARTpos2);//40
    one_1Dhist("count bad ART", "channel", hARTposcountBad, 0);//41
    two_1Dhist("RMS", "count channels", "RMS", "count channels",hPDORMS_L0 ,hPDORMS_L0POS);//42 and 43   
    one_1Dhist("RMS", "count channels", hblineRMS, 1);//44
    two_1Dhist("b", "count channels", "b", "count channels", hPDOIntercept_L0, hPDOIntercept_L0POS);//45 and 46
    two_1Dhist("a", "count channels", "a", "count channels", hPDOSlope_L0, hPDOSlope_L0POS);//47 and 48
    two_1Dhist("RMS", "count channels", "RMS", "count channels", hTDORMS_L0, hTDORMS_L0POS);//49 and 50
    two_1Dhist("b", "count channels", "b", "count channels", hTDOIntercept_L0, hTDOIntercept_L0POS);//51 and 52
    two_1Dhist("a", "count channels", "a", "count channels", hTDOSlope_L0, hTDOSlope_L0POS);//53 and 54
    Canv->Print(pdfclose, "pdf");
    Canv->Clear();
    Canv->Delete();

}

void convert_to_date(char* outStr, UInt_t unixTime, int formattype){ //FormatType 1 = ddmmyyy, FormatType 2 = dd/mm/yyy
    time_t dateTime_time_t = static_cast<time_t>(unixTime); //converts uint to time_t format
    struct tm * timeinfo; //Another format of time
    char myTime[20]; //char, where I will store my time in needed format
    timeinfo = localtime (&dateTime_time_t); //convert UNIX time to tm structure format
    if (formattype == 1)
        strftime (myTime, 20, "%d%m20%y", timeinfo); //store desired format in my char myTime
    if (formattype == 2)
        strftime (myTime, 20, "%d/%m/20%y", timeinfo);
    for (int i = 0; i<20; i++){
        outStr[i] = myTime[i];
    }
}

int correct_VMMId(int number, uint dateTime)
{
    if (dateTime < 1556668800){ //For chips, that consist of 4 numbers (FR3018, FR3118)
        while (number > 9999){ // If VMMId is bigger than 4 numbers
            number = number - 10000;
        }
    }
    else{ 
        while (number > 99999){ // If VMMId is bigger than 4 numbers (KR0819 series)
            number = number - 100000;
        }
    }
    return number;
}

void fillFlags(int arr[][11], int arrSize, int ChipStatus_col, int n_flag_col, int n_dead_ch_col, int neg_or_pos){
    //First. If there are two or more measurements, and at least one is measured after 19 July 2019, the flag of others become 0
    for (i = 0; i<arrSize; i++){
        if (arr[i][8] < 1532000000){
           for (j = 0; j<arrSize; j++){
               if (arr[i][0] == arr[j][0] && arr[j][8] > 1532000000){
                   arr[i][n_flag_col] = 3;
                   break;
               }
           }
        }
    } 
    //Second. We sort entries on flags 0, 1, 2
    for (i = 0; i<arrSize; i++){
        int current_status = arr[i][ChipStatus_col]; //best status of a certain VMM3a
        if (arr[i][n_flag_col] != 3)//If this meas was not sorted out in the first step
            arr[i][n_flag_col] = 1; //By default the Flag is equal to 1
        for (j = 0; j<arrSize; j++){ //Searching again in this array - if this chip was measured again
            if (i != j && arr[i][0] == arr[j][0] && arr[i][n_flag_col] != 3 && arr[j][n_flag_col] != 3){ //If there is Entry with the same chipID and this measurement wasnt sorted out in the first step
                if (current_status < arr[j][ChipStatus_col]) //If there is any measurement, where status were higher, at this moment current measurement looses it flag1 (turn to be flag0)
                    arr[i][n_flag_col] = 0;
                if (arr[i][n_flag_col] != 0 && current_status == arr[j][ChipStatus_col]){ // If it is not lost it flag1 yet, and find the entry with the same status, it will have flag 2 (to know, that there are two or more measurements with the same status
                    arr[i][n_flag_col] = 2;
                }
            }
        }
    }
    //Third. We sort entries with flag 2 on flags 20, 21, 22
    for (i = 0; i<arrSize; i++){
        if (arr[i][n_flag_col] == 2){
            arr[i][n_flag_col] = 21; //By default the flag turn to be 21
            for (j = 0; j<arrSize; j++){
                if (i != j && arr[i][0] == arr[j][0] && (arr[j][n_flag_col] == 2 || arr[j][n_flag_col] == 20 || arr[j][n_flag_col] == 21 || arr[j][n_flag_col] == 22)){ // If there is a entry for the same chip with the flag, started with 2
                    if (arr[i][n_dead_ch_col] > arr[j][n_dead_ch_col])//Compare the number of dead channel. If the number of dead chanel in this entry more than in other
                        arr[i][n_flag_col] = 20; //The flag turns to 20
                    if (arr[i][n_flag_col] != 20 && arr[i][n_dead_ch_col] == arr[j][n_dead_ch_col]){// If it is not lost it flag21 yet, and find the entry with the same number of dead channels, it will have flag 22 (to know, that there are two or more measurements with the same flag 2 and the same number of dead chanels
                        arr[i][n_flag_col] = 22;
                    }
                }
            }
        }
    }
    //Fourth. We sort entries with flag 22 on flags 220, 221
    for (i = 0; i<arrSize; i++){
        if (arr[i][n_flag_col] == 22){
            arr[i][n_flag_col] = 221; //By default the flag turn to be 221
            for (j = 0; j<arrSize; j++){
                if (i != j && arr[i][0] == arr[j][0] && (arr[j][n_flag_col] == 22 || arr[j][n_flag_col] == 220 || arr[j][n_flag_col] == 221)){ // If there is a entry for the same chip with the flag, started with 22
                    if (arr[i][8] < arr[j][8])//Compare the number datetime. If the number of datetime in this entry is less than in other
                        arr[i][n_flag_col] = 220; //The flag turns to 220
                }
            }
        }
    }
    //Fifth. We sort entries with flag 221 (Entries that have the same datetime) on flags 220, 221
    for (i = 0; i<arrSize; i++){
        if (arr[i][n_flag_col] == 221){
            for (j = 0; j<arrSize; j++){
                if (i != j && arr[i][0] == arr[j][0] && arr[j][n_flag_col] == 221){ // If there is a entry for the same chip with the flag, started with 221
                    if (i > j){//Compare the order of the entry. If the number is more than in other file
                        arr[i][n_flag_col] = 220; //The flag turns to 220
                        break;
                    }
                }
            }
        }
    }
}


int calculate_Total_Stat(int arr[][11], int arrSize, int VMMID){
    int totalStatus = 0;
    for (int i = 0; i<arrSize; i++){
        if (arr[i][0] == VMMID){
            totalStatus += arr[i][9]; //Total Status is a sum of General Statuses
        }
    }
    return totalStatus;
}

int calculate_Total_Stat_two_Polarities(int arr[][11], int arrSize, int VMMID){
    int StatTotal, StatNeg, StatPos;
    for (int i = 0; i<arrSize; i++){
        if (arr[i][0] == VMMID){
            if (arr[i][4] == 1 || arr[i][4] == 21 || arr[i][4] == 221)  //Search best status of Neg Polarity
                StatNeg = arr[i][2];
            if (arr[i][5] == 1 || arr[i][5] == 21 || arr[i][5] == 221)  //Search best status of Neg Polarity
                StatPos = arr[i][3];
        if (StatNeg == 2 && StatPos == 2)
            StatTotal = 4;
        if (StatNeg == 2 && StatPos == 1)
            StatTotal = 3;
        if (StatNeg == 1 && StatPos == 2)
            StatTotal = 2;
        if (StatNeg == 1 && StatPos == 1)
            StatTotal = 1;
        if (StatNeg == 0 || StatPos == 0)
            StatTotal = 0;
        }
    }
    return StatTotal;
}
//NegDeadChCount arr[i][6]  PosDeadChCount arr[i][7]  NegStatus arr[i][2]  PosStatus arr[i][3] numberBadCh_ARTneg
int write_to_Global(int arr[][11], int arrSize){
    int Status;
    int flag_good_or_bad, flag_good_or_bad_ART;
    fileo<<"SN\tStatus\tPartID\tFPGAver\t\t"; //General fields
    fileo<<"NegTime\t\tNegDeadChCount\tNegStatus\tNegBoardID\t"; //Negative field
    fileo<<"PosTime\t\tPosDeadChCount\tPosStatus\tPosBoardID"<<endl; //Positive fields
    fileo_dead_ch<<"NegDeadChannels\t\t\t\t\t\t\t\tPosDeadChannels"<<endl;
    fileo_dead_ch_Bline<<"NegDeadChannels\t\t\t\t\t\t\t\tPosDeadChannels"<<endl;
    fileo_dead_ch_ART<<"NegDeadChannels\t\t\t\t\t\t\t\tPosDeadChannels"<<endl;
    fileo_dead_ch_PDO<<"NegDeadChannels\t\t\t\t\t\t\t\tPosDeadChannels"<<endl;
    fileo_dead_ch_TDO<<"NegDeadChannels\t\t\t\t\t\t\t\tPosDeadChannels"<<endl;
    fileo_dead_ch_DDO<<"NegDeadChannels\t\t\t\t\t\t\t\tPosDeadChannels"<<endl;

    for (i = 0; i<arrSize; i++){
        Status = calculate_Total_Stat_two_Polarities(arr, arrSize, arr[i][0]);
        if ((arr[i][4] == 1) || (arr[i][4] == 21) || (arr[i][4] == 221)){ //if FlagNeg is good
                roottree->GetEntry(i);
	        fileo<<arr[i][0]<<"\t"<<Status<<"\t"<<partID<<"\t"<<FPGA_VER<<"\t"; //General fields data
	        fileo<<arr[i][8]<<"\t"<<arr[i][6]<<"\t\t"<<arr[i][2]<<"\t\t"<<arr[i][10]<<"\t\t"; //Negative fields data
                //Now, lets write the information about dead channels_Negative
                for (k = 0; k<64; k++){
                    flag_good_or_bad = 1; //By default, the channel is OKi
                    flag_good_or_bad_ART = 1;
                    for (vi = neg_DEAD_CHANNELS->begin(); vi != neg_DEAD_CHANNELS->end(); ++vi){ //Write which channels from 64 are bad
                        if (*vi == k){ //If channel has been found in bad, flag turns 0
                            flag_good_or_bad = 0;
                            break;
                        }
                    }
                    //Bad_by_ART
                    for (vi = numberBadCh_ARTneg->begin(); vi != numberBadCh_ARTneg->end(); ++vi){ //Write which channels from 64 are bad
                        if (*vi == k){ //If channel has been found in bad, flag turns 0
                            flag_good_or_bad_ART = 0;
                            break;
                        }
                    }
                    print_to_log_to_Global(EntriesARTneg[k], fileo_ART_entries);
                    //Bad_by_PDO
                    if (PDOMEAN_L0NEG[k] < 200 || PDOMEAN_L0NEG[k] > 1022 || PDOEntriesMEAN_L0NEG[k] < 50){
                        fileo_dead_ch_PDO<<"0";
                    }
                    else
                        fileo_dead_ch_PDO<<"1";
                    print_to_log_to_Global(PDOMEAN_L0NEG[k], fileo_PDO_mean);
                    print_to_log_to_Global(PDOEntriesMEAN_L0NEG[k], fileo_PDO_entries);
                    //Bad_by_TDO
                    if (TDOMEAN_L0NEG[k] < 32 || TDOMEAN_L0NEG[k] > 254 || TDOEntriesMEAN_L0NEG[k] < 50){
                        fileo_dead_ch_TDO<<"0";
                    }
                    else
                        fileo_dead_ch_TDO<<"1";
                    print_to_log_to_Global(TDOMEAN_L0NEG[k], fileo_TDO_mean);
                    print_to_log_to_Global(TDOEntriesMEAN_L0NEG[k], fileo_TDO_entries);
                    //Bad_by_DDO
                    if (MEAN_DDOneg[k] < 2 || MEAN_DDOneg[k] > 62)
                        fileo_dead_ch_DDO<<"0";
                    else
                        fileo_dead_ch_DDO<<"1";
                    print_to_log_to_Global(MEAN_DDOneg[k], fileo_DDO_mean);
                    fileo_dead_ch<<flag_good_or_bad;
                    fileo_dead_ch_ART<<flag_good_or_bad_ART;
                }
                //Tab to switch to Positive
                fileo_dead_ch<<"\t";
                fileo_dead_ch_ART<<"\t";
                fileo_dead_ch_PDO<<"\t";
                fileo_dead_ch_TDO<<"\t";
                fileo_dead_ch_DDO<<"\t";
                fileo_PDO_mean<<"\t";
                fileo_TDO_mean<<"\t";
                fileo_PDO_entries<<"\t";
                fileo_TDO_entries<<"\t";
                fileo_DDO_mean<<"\t";
                fileo_ART_entries<<"\t";
                //Bad_by_Bline
                if (bLineNeg[0] >= 0){ //If Bline file exists
                    for (k = 0; k<64; k++){
                        if ((bLineNeg[k] > 135) && (bLineNeg[k] < 195)) 
                            fileo_dead_ch_Bline<<"1";
                        else
                            fileo_dead_ch_Bline<<"0";
                        fileo_Bline<<bLineNeg[k]<<" ";
                    }
                }
                else{ //If Bline file is absent
                    fileo_dead_ch_Bline<<"----------------------------------------------------------------";
                    fileo_Bline<<"----------------------------------------------------------------";
                }
                fileo_dead_ch_Bline<<"\t";
                fileo_Bline<<"\t";
                //Search again to Fill in the Positive fields
                for (j = 0; j<arrSize; j++){
                    if ((arr[i][0] == arr[j][0]) && ((arr[j][5] == 1) || (arr[j][5] == 21) || (arr[j][5] == 221))){ //if FlagPos is good for this chip
                        roottree->GetEntry(j);
		        fileo<<arr[j][8]<<"\t"<<arr[j][7]<<"\t\t"<<arr[j][3]<<"\t\t"<<arr[j][10]<<"\t\t"<<endl; //Positive fields data
                        //Now, lets write the information about dead channels_Posititve
                        for (k = 0; k<64; k++){
                            flag_good_or_bad = 1; //By default, the channel is OK
                            flag_good_or_bad_ART = 1;
                            for (vi = pos_DEAD_CHANNELS->begin(); vi != pos_DEAD_CHANNELS->end(); ++vi){ //Write which channels from 64 are bad
                                if (*vi == k){   //If channel has been found in bad, flag turns 0
                                    flag_good_or_bad = 0;
                                    break;
                                }
                            }
                            //Bad_by_ART
                            for (vi = numberBadCh_ARTpos->begin(); vi != numberBadCh_ARTpos->end(); ++vi){ //Write which channels from 64 are bad
                                if (*vi == k){ //If channel has been found in bad, flag turns 0
                                    flag_good_or_bad_ART = 0;
                                    break;
                                }
                            }
                            print_to_log_to_Global(EntriesARTpos[k], fileo_ART_entries);
                            //Bad_by_PDO
                            if (PDOMEAN_L0POS[k] < 200 || PDOMEAN_L0POS[k] > 1022 || PDOEntriesMEAN_L0POS[k] < 50)
                                fileo_dead_ch_PDO<<"0";
                            else
                                fileo_dead_ch_PDO<<"1";
                            print_to_log_to_Global(PDOMEAN_L0POS[k], fileo_PDO_mean);
                            print_to_log_to_Global(PDOEntriesMEAN_L0POS[k], fileo_PDO_entries);
                            //Bad_by_TDO
                            if (TDOMEAN_L0POS[k] < 32 || TDOMEAN_L0POS[k] > 254 || TDOEntriesMEAN_L0POS[k] < 50)
                                fileo_dead_ch_TDO<<"0";
                            else
                               fileo_dead_ch_TDO<<"1";
                            print_to_log_to_Global(TDOMEAN_L0POS[k], fileo_TDO_mean);
                            print_to_log_to_Global(TDOEntriesMEAN_L0POS[k], fileo_TDO_entries);
                            //Bad_by_DDO
                            if (MEAN_DDOpos[k] < 2 || MEAN_DDOpos[k] > 62)
                                fileo_dead_ch_DDO<<"0";
                            else
                               fileo_dead_ch_DDO<<"1";
                            print_to_log_to_Global(MEAN_DDOpos[k], fileo_DDO_mean);
                            fileo_dead_ch<<flag_good_or_bad;
                            fileo_dead_ch_ART<<flag_good_or_bad_ART;
                        }
                        //Write Bline and  dead channels in Bline mode
                        if (bLinePos[0] >= 0){ //If Bline file exists
                            for (k = 0; k<64; k++){
                                if ((bLinePos[k] > 135) && (bLinePos[k] < 195))
                                    fileo_dead_ch_Bline<<"1";
                                else
                                    fileo_dead_ch_Bline<<"0";
                                fileo_Bline<<bLineNeg[k]<<" ";
                            }
                        }
                        else{ //If Bline file is absent
                            fileo_dead_ch_Bline<<"----------------------------------------------------------------";
                            fileo_Bline<<"----------------------------------------------------------------";
                        }
                        fileo_dead_ch_Bline<<endl;
                        fileo_Bline<<endl;
                        fileo_dead_ch<<endl;
                        fileo_dead_ch_ART<<endl;
                        fileo_dead_ch_PDO<<endl;
                        fileo_dead_ch_TDO<<endl;
                        fileo_dead_ch_DDO<<endl;
                        fileo_TDO_mean<<endl;
                        fileo_TDO_entries<<endl;
                        fileo_PDO_mean<<endl;
                        fileo_PDO_entries<<endl;
                        fileo_DDO_mean<<endl;
                        fileo_ART_entries<<endl;
                        break; //If the positive data for this chip are found - stop searching
                    }
                    else if (j == (arrSize - 1)){ //If we didn't find the positive results for the chip
                        fileo<<endl; //End lines in all files
                        fileo_dead_ch<<endl;
                        fileo_dead_ch_Bline<<endl;
                        fileo_dead_ch_ART<<endl;
                        fileo_dead_ch_PDO<<endl;
                        fileo_dead_ch_TDO<<endl;
                        fileo_dead_ch_DDO<<endl;
                        fileo_Bline<<endl;
                        fileo_TDO_mean<<endl;
                        fileo_TDO_entries<<endl;
                        fileo_PDO_mean<<endl;
                        fileo_PDO_entries<<endl;
                        fileo_DDO_mean<<endl;
                        fileo_ART_entries<<endl;
                    }
                }
        }
    }
    return 0; 
}
int print_to_log_to_Global(float value, ofstream& file){
    if (value >= 0) //If enty exists
        file<<value<<" "; //print this value
    else
        file<<"- ";
}
void write_to_Database(int arr[][11], int arrSize, int neg_or_pos) //neg = 0, pos = 1
{   
    TString path_to_web;
    int StatTotal_twoPolarities;
    counter = 0;
    if (neg_or_pos == 0){
        textBig = "NEG";
        textSmall = "neg";
        textDefault = "";
    }
    else{
        textBig = "POS";
        textSmall = "pos";
        textDefault = "pos";
        
    }
    for (i = 0; i<arrSize; i++){
        k = 4 + neg_or_pos;
        l = 2 + neg_or_pos;
        if (arr[i][k] == 1 || arr[i][k] == 21 || arr[i][k] == 221){
            roottree->GetEntry(i);
            if (dateTime < 1554076800)
                path_to_web = path_to_web_FR;
                
            else
                path_to_web = path_to_web_KR;
            StatTotal_twoPolarities = calculate_Total_Stat_two_Polarities(arr, arrSize, correct_VMMId(VMMId, dateTime));
            Total_Status = calculate_Total_Stat(arr, arrSize, correct_VMMId(VMMId, dateTime));
            fileo<<"chip_id : "<<arr[i][0]<<" "<<partID<<" ";
            if (StatTotal_twoPolarities == 4)
                fileo<<"Perfect ";
            if (StatTotal_twoPolarities == 3)
                fileo<<"PerfectNEG ";
            if (StatTotal_twoPolarities == 2)
                fileo<<"PerfectPOS ";
            if (StatTotal_twoPolarities == 1)
                fileo<<"Passed ";
            if (StatTotal_twoPolarities == 0)
                fileo<<"FAILED ";
            fileo<<Total_Status<<endl;
            fileo<<"CONF : "<<textBig<<endl;
            if (arr[i][l] == 0)
                fileo<<"Status : Failed"<<endl;
            if (arr[i][l] == 1)
                fileo<<"Status : Passed"<<endl;
            if (arr[i][l] == 2)
                fileo<<"Status : Perfect"<<endl;
            fileo<<"failed : "<<FAILED<<endl;
            //This block is needed to format link in a correct way
            if (VMMId < 1000)
                VMMID_for_link[0] = '0';
            if (VMMId < 100)
                VMMID_for_link[1] = '0';
            if (VMMId < 10)
                VMMID_for_link[2] = '0';
            fileo<<path_to_web<<folderTesting<<"/constants/"<<chipStatus<<"_const_"<<VMMID_for_link<<VMMId<<textBig<<".txt"<<endl;
            fileo<<path_to_web<<folderTesting<<"/pdf/"<<VMMID_for_link<<VMMId<<textBig<<".pdf"<<endl;
            if (neg_or_pos == 0){
                if (bLineNeg[0] >= 0)
                    fileo<<path_to_web<<folderTesting<<"/root/"<<VMMID_for_link<<VMMId<<"_BLine"<<textSmall<<".root"<<endl;
                else if (bLinePos[0] >= 0)
                    fileo<<path_to_web<<folderTesting<<"/root/"<<VMMID_for_link<<VMMId<<"_BLine"<<"pos"<<".root"<<endl;
            }
            else{
                if (bLinePos[0] >= 0)
                    fileo<<path_to_web<<folderTesting<<"/root/"<<VMMID_for_link<<VMMId<<"_BLine"<<textSmall<<".root"<<endl;
                else if (bLineNeg[0] >= 0)
                    fileo<<path_to_web<<folderTesting<<"/root/"<<VMMID_for_link<<VMMId<<"_BLine"<<"neg"<<".root"<<endl;
            }
            fileo<<path_to_web<<folderTesting<<"/root/"<<VMMID_for_link<<VMMId<<"_ART"<<textSmall<<".root"<<endl;
            fileo<<path_to_web<<folderTesting<<"/root/"<<VMMID_for_link<<VMMId<<"_L0"<<textDefault<<"_PDO.root"<<endl;
            fileo<<path_to_web<<folderTesting<<"/root/"<<VMMID_for_link<<VMMId<<"_L0"<<textDefault<<"_TDO.root"<<endl;
            fileo<<path_to_web<<folderTesting<<"/root/"<<VMMID_for_link<<VMMId<<"_DDO"<<textSmall<<".root"<<endl;
            
            fileo<<"**********************************************************"<<endl;
            //counter++;
            for (j = 0; j<3; j++){
                VMMID_for_link[j] = '\0';
            }
        }
    }
}

void write_to_Certificate(int arr[][11], int arrSize, int neg_or_pos, int FR_KR) //neg = 0, pos = 1	FR = 0, KR = 1
{
    float VDDP_A, VDD_A, VDDD_A, VDAD_A, blineValue, meanValue, entries;
    //Variables for statistics
    int Quantity_neg[7], Quantity_Perfect[7], Quantity_PerfectNEG[7], Quantity_PerfectPOS[7], Quantity_Passed[7], Quantity_Failed[7], Quantity_Currents[7], Quantity_Baseline[7], Quantity_PDO[7], Quantity_TDO[7], Quantity_DDO[7], Quantity_ART[7];
    float ichar;
    float Percent_Neg[7], Percent_Perfect[7], Percent_PerfectNEG[7], Percent_PerfectPOS[7], Percent_Passed[7], Percent_Failed[7], Percent_Currents[7], Percent_Baseline[7], Percent_PDO[7], Percent_TDO[7], Percent_DDO[7], Percent_ART[7];
    float Fract_part, Int_part;
    int startdate, stopdate;
    int StatTotal_twoPolarities;
    int flag_mixed;
    int dead_sum;
    for (i = 0; i<7; i++){
        Quantity_neg[i] = 0;
        Quantity_Perfect[i] = 0;
        Quantity_PerfectNEG[i] = 0;
        Quantity_PerfectPOS[i] = 0;
        Quantity_Passed[i] = 0;
        Quantity_Failed[i] = 0;
        Percent_Perfect[i] = 0;
        Percent_PerfectNEG[i] = 0;
        Percent_PerfectPOS[i] = 0;
        Percent_Passed[i] = 0;
        Percent_Failed[i] = 0;
        Quantity_Currents[i] = 0;
        Quantity_Baseline[i] = 0;
        Quantity_PDO[i] = 0;
        Quantity_TDO[i] = 0;
        Quantity_DDO[i] = 0;
        Quantity_ART[i] = 0;
        Percent_Currents[i] = 0;
        Percent_Baseline[i] = 0;
        Percent_PDO[i] = 0;
        Percent_TDO[i] = 0;
        Percent_DDO[i] = 0;
        Percent_ART[i] = 0;
        Percent_Neg[i] = 0;
    }
    counter = 0;
    if (neg_or_pos == 0){
        textBig = "NEG";
        textSmall = "neg";
    }
    else{
        textBig = "POS";
        textSmall = "pos";
        
    }
    if (FR_KR == 0){
        startdate = 1462060800; //1 May 2016
        stopdate = 1556668800; //1 May 2019
    }
    else{
        startdate = 1557784800; //14 May 2019
        stopdate = 1651363200; //1 May 2022
        //startdate = 1558648800; //24 MAy
        //stopdate = 1558735200; //25 May
    }
    //Start of Statistics block
    for (i = 0; i<arrSize; i++){
        if ((arr[i][8] > startdate) && (arr[i][8] < stopdate)){
            if (arr[i][4] == 1 || arr[i][4] == 21 || arr[i][4] == 221){
                Quantity_neg[0] ++;
                Quantity_neg[arr[i][10]] ++;
                StatTotal_twoPolarities = calculate_Total_Stat_two_Polarities(arr, arrSize, arr[i][0]);
                if (StatTotal_twoPolarities == 4){
                    Quantity_Perfect[0] ++;
                    Quantity_Perfect[arr[i][10]] ++;
                }
                if (StatTotal_twoPolarities == 3){
                    Quantity_PerfectNEG[0] ++;
                    Quantity_PerfectNEG[arr[i][10]] ++;
                }
                if (StatTotal_twoPolarities == 2){
                    Quantity_PerfectPOS[0] ++;
                    Quantity_PerfectPOS[arr[i][10]] ++;
                }
                if (StatTotal_twoPolarities == 1){
                    Quantity_Passed[0] ++;
                    Quantity_Passed[arr[i][10]] ++;
                }
                if (StatTotal_twoPolarities == 0){
                    Quantity_Failed[0] ++;
                    Quantity_Failed[arr[i][10]] ++;
                    roottree->GetEntry(i);
                    flag = 0;
                    for (j = 0; j<64; j++){
                        if (FAILED[j] == 'C'){ //If the chip was failed by the current
                            Quantity_Currents[0] ++;
                            Quantity_Currents[arr[i][10]] ++;
                            flag = 1;
                            break;
                        }
                    }
                    if (flag == 0){ //If the are no problems with current filter
                        for (j = 0; j<64; j++){
                            if (FAILED[j] == 'B'){
                                ichar = FAILED[j+2] - '0'; //convert char to int
                                if ((ichar > 2) || isdigit(FAILED[j+3])){     //If the number of dead chips is more than two
                                    Quantity_Baseline[0] ++;
                                    Quantity_Baseline[arr[i][10]] ++;
                                    flag = 1;
                                    break;
                                }
                            }
                        }
                    }
                    if (flag == 0){ //If the are no problems with baseline filter
                        for (j = 0; j<64; j++){
                            if (FAILED[j] == 'P'){
                                ichar = FAILED[j+2] - '0'; //convert char to int
                                if ((ichar > 2) || isdigit(FAILED[j+3])){     //If the number of dead chips is more than two
                                    Quantity_PDO[0] ++;
                                    Quantity_PDO[arr[i][10]] ++;
                                    flag = 1;
                                    break;
                                }
                            }
                        }
                    }
                    if (flag == 0){ //If the are no problems with PDO filter
                        for (j = 0; j<64; j++){
                            if (FAILED[j] == 'T'){
                                ichar = FAILED[j+2] - '0'; //convert char to int
                                if ((ichar > 2) || isdigit(FAILED[j+3])){     //If the number of dead chips is more than two
                                    Quantity_TDO[0] ++;
                                    Quantity_TDO[arr[i][10]] ++;
                                    flag = 1;
                                    break;
                                }
                            }
                        }
                    }
                    if (flag == 0){ //If the are no problems with TDO filter
                        for (j = 0; j<64; j++){
                            if (FAILED[j] == 'A'){
                                ichar = FAILED[j+2] - '0'; //convert char to int
                                if ((ichar > 2) || isdigit(FAILED[j+3])){     //If the number of dead chips is more than two
                                    Quantity_ART[0] ++;
                                    Quantity_ART[arr[i][10]] ++;
                                    flag = 1;
                                    break;
                                }
                            }
                        }
                    }
                    if (flag == 0){ //If the are no problems with ART filter
                        for (j = 0; j<64; j++){
                            if (FAILED[j] == 'D'){
                                ichar = FAILED[j+2] - '0'; //convert char to int
                                if ((ichar > 2) || isdigit(FAILED[j+3])){     //If the number of dead chips is more than two
                                    Quantity_DDO[0] ++;
                                    Quantity_DDO[arr[i][10]] ++;
                                    flag = 1;
                                    break;
                                }
                            }
                        }
                    }
                    if (flag == 0){ //If chip is not Failed in any of the criteria (in case of in summary three chanel are failed, but all in different reasons. In this case, we will take the reason where the third channel appers
                        dead_sum = 0;
                        for (j = 0; j<64; j++){
                            if (FAILED[j] == 'B'){
                                ichar = FAILED[j+2] - '0';
                                if (FAILED[j+1] == '-'){
                                    dead_sum = dead_sum + ichar;
                                }
                            }
                            if (FAILED[j] == 'P'){
                                ichar = FAILED[j+2] - '0';
                                if (FAILED[j+1] == '-'){
                                    dead_sum = dead_sum + ichar;
                                }
                            }
                            if (dead_sum < 3){
                                if (FAILED[j] == 'T'){
                                   ichar = FAILED[j+2] - '0';
                                   if (FAILED[j+1] == '-'){
                                       dead_sum = dead_sum + ichar;
                                   }
                                }
                            
                            }
                            else{
                                Quantity_PDO[0] ++;
                                Quantity_PDO[arr[i][10]] ++;
                                flag = 1;
                                break;
                            }
                            if (dead_sum < 3){
                                if (FAILED[j] == 'A'){
                                   ichar = FAILED[j+2] - '0';
                                   if (FAILED[j+1] == '-'){
                                       dead_sum = dead_sum + ichar;
                                   }
                                }
                            }
                            else{
                                Quantity_TDO[0] ++;
                                Quantity_TDO[arr[i][10]] ++;
                                flag = 1;                                    
                                break;
                            }
                            if (dead_sum < 3){
                                if (FAILED[j] == 'D'){
                                   ichar = FAILED[j+2] - '0';
                                   if (FAILED[j+1] == '-'){
                                       dead_sum = dead_sum + ichar;
                                   }
                                }
                            }
                            else{
                                Quantity_ART[0] ++;
                                Quantity_ART[arr[i][10]] ++;
                                flag = 1;
                                break;
                            }
                            if (dead_sum > 2){
                                Quantity_DDO[0] ++;
                                Quantity_DDO[arr[i][10]] ++;
                                flag = 1;
                                break;
                            }
                        }
                    }
                    if (flag == 0){ //If chip is not Failed in any of the criteria (in case of in summary three chanel are failed, but all in different reasons. In this case, we will take the reason where the third channel appers
                        dead_sum = 0;
                        for (j = 0; j<64; j++){
                            if (FAILED[j] == 'B'){
                                ichar = FAILED[j+2] - '0';
                                if (FAILED[j+1] == '+'){
                                    dead_sum = dead_sum + ichar;
                                }
                            }
                            if (FAILED[j] == 'P'){
                                ichar = FAILED[j+2] - '0';
                                if (FAILED[j+1] == '+'){
                                    dead_sum = dead_sum + ichar;
                                }
                            }
                            if (dead_sum < 3){
                                if (FAILED[j] == 'T'){
                                   ichar = FAILED[j+2] - '0';
                                   if (FAILED[j+1] == '+'){
                                       dead_sum = dead_sum + ichar;
                                   }
                                }

                            }
                            else{
                                Quantity_PDO[0] ++;
                                Quantity_PDO[arr[i][10]] ++;
                                flag = 1;
                                break;
                            }
                            if (dead_sum < 3){
                                if (FAILED[j] == 'A'){
                                   ichar = FAILED[j+2] - '0';
                                   if (FAILED[j+1] == '+'){
                                       dead_sum = dead_sum + ichar;
                                   }
                                }
                            }
                            else{
                                Quantity_TDO[0] ++;
                                Quantity_TDO[arr[i][10]] ++;
                                flag = 1;
                                break;
                            }
                            if (dead_sum < 3){
                                if (FAILED[j] == 'D'){
                                   ichar = FAILED[j+2] - '0';
                                   if (FAILED[j+1] == '+'){
                                       dead_sum = dead_sum + ichar;
                                   }
                                }
                            }
                            else{
                                Quantity_ART[0] ++;
                                Quantity_ART[arr[i][10]] ++;
                                flag = 1;
                                break;
                            }
                            if (dead_sum > 2){
                                Quantity_DDO[0] ++;
                                Quantity_DDO[arr[i][10]] ++;
                                flag = 1;
                                break;
                            }
                        }
                    }
                    //if (flag == 0)
                    //    cout<<FAILED<<" "<<arr[i][0]<<endl;
                    for (j = 0; j<64; j++)
                        FAILED[j] = ' ';
                }
            }
        }
    }
    for (i=0; i<7; i++){
        if ((Quantity_neg[i] != 0) && i != 0) //For all events (except total amoun of events
            Percent_Neg[i] = (float)Quantity_neg[i]/Quantity_neg[i] * 100; 
        if (Quantity_Perfect[i] != 0)
            Percent_Perfect[i] = (float)Quantity_Perfect[i]/Quantity_neg[i] * 100;
        if (Quantity_PerfectNEG[i] != 0)
            Percent_PerfectNEG[i] =(float)Quantity_PerfectNEG[i]/Quantity_neg[i]*100;
        if (Quantity_PerfectPOS[i] != 0)
            Percent_PerfectPOS[i] =(float)Quantity_PerfectPOS[i]/Quantity_neg[i]*100;
        if (Quantity_Passed[i] != 0)
            Percent_Passed[i] =(float)Quantity_Passed[i]/Quantity_neg[i]*100;
        if (Quantity_Failed[i] != 0)
            Percent_Failed[i] =(float)Quantity_Failed[i]/Quantity_neg[i]*100;
    }
    Percent_Neg[0] = 100;
    fileo<<"|-----------------------------------------------------------------------------------------------|"<<endl;
    fileo<<"|\t\t|Total\t\t|Board 1\t|Board 2\t|Board 5\t|Board 6\t|"<<endl;
    fileo<<"|-----------------------------------------------------------------------------------------------|"<<endl;
    fileo<<"| Quantity\t";
    for (i=0; i<7; i++)
        if ((i != 3) && (i != 4)){
            Fract_part = 10 * modf(Percent_Neg[i], &Int_part);
            fileo<<"|"<<setw(4)<<setfill(' ')<<Quantity_neg[i]<<" ("<<setw(3)<<setfill(' ')<<int(Percent_Neg[i])<<"."<<int(Fract_part)<<"%)"<<"\t";
        }
    fileo<<"|"<<endl;
    fileo<<"| Perfect\t";
    for (i=0; i<7; i++)
        if ((i != 3) && (i != 4)){
            Fract_part = 10 * modf(Percent_Perfect[i], &Int_part);    
            fileo<<"|"<<setw(4)<<setfill(' ')<<Quantity_Perfect[i]<<" ("<<setw(3)<<setfill(' ')<<int(Percent_Perfect[i])<<"."<<int(Fract_part)<<"%)"<<"\t";
        }
    fileo<<"|"<<endl;
    fileo<<"| PerfectNEG\t";
    for (i=0; i<7; i++)
        if ((i != 3) && (i != 4)){
            Fract_part = 10 * modf(Percent_PerfectNEG[i], &Int_part);
            fileo<<"|"<<setw(4)<<setfill(' ')<<Quantity_PerfectNEG[i]<<" ("<<setw(3)<<setfill(' ')<<int(Percent_PerfectNEG[i])<<"."<<int(Fract_part)<<"%)"<<"\t";
        }
    fileo<<"|"<<endl;
    fileo<<"| PerfectPOS\t";
    for (i=0; i<7; i++)
        if ((i != 3) && (i != 4)){
            Fract_part = 10 * modf(Percent_PerfectPOS[i], &Int_part);
            fileo<<"|"<<setw(4)<<setfill(' ')<<Quantity_PerfectPOS[i]<<" ("<<setw(3)<<setfill(' ')<<int(Percent_PerfectPOS[i])<<"."<<int(Fract_part)<<"%)"<<"\t";
        }
    fileo<<"|"<<endl;
    fileo<<"| Passed\t";
    for (i=0; i<7; i++)
        if ((i != 3) && (i != 4)){
            Fract_part = 10 * modf(Percent_Passed[i], &Int_part);
            fileo<<"|"<<setw(4)<<setfill(' ')<<Quantity_Passed[i]<<" ("<<setw(3)<<setfill(' ')<<int(Percent_Passed[i])<<"."<<int(Fract_part)<<"%)"<<"\t";
        }
    fileo<<"|"<<endl;
    fileo<<"| Failed\t";
    for (i=0; i<7; i++)
        if ((i != 3) && (i != 4)){
            Fract_part = 10 * modf(Percent_Failed[i], &Int_part);
            fileo<<"|"<<setw(4)<<setfill(' ')<<Quantity_Failed[i]<<" ("<<setw(3)<<setfill(' ')<<int(Percent_Failed[i])<<"."<<int(Fract_part)<<"%)"<<"\t";
        }
    fileo<<"|"<<endl;

    if (Quantity_Failed[0] != 0){
        for (i=0; i<7; i++){
            if (Quantity_Currents[i] != 0)
                Percent_Currents[i] = (float)Quantity_Currents[i]/Quantity_Failed[i] * 100;
            if (Quantity_Baseline[i] != 0)
                Percent_Baseline[i] =(float)Quantity_Baseline[i]/(float)Quantity_Failed[i]*100;
            if (Quantity_PDO[i] != 0)
                Percent_PDO[i] =(float)Quantity_PDO[i]/Quantity_Failed[i]*100;
            if (Quantity_TDO[i] != 0)
                Percent_TDO[i] =(float)Quantity_TDO[i]/Quantity_Failed[i]*100;
            if (Quantity_ART[i] != 0)
                Percent_ART[i] =(float)Quantity_ART[i]/Quantity_Failed[i]*100;
            if (Quantity_DDO[i] != 0)
                Percent_DDO[i] =(float)Quantity_DDO[i]/Quantity_Failed[i]*100;      
        } 
    }
    fileo<<"================================================================================================="<<endl;
    fileo<<"| Failed by priorities"<<endl;
    fileo<<"|-----------------------------------------------------------------------------------------------|"<<endl;
    fileo<<"|\t\t|Total\t\t|Board 1\t|Board 2\t|Board 5\t|Board 6\t|"<<endl;
    fileo<<"|-----------------------------------------------------------------------------------------------|"<<endl;
    fileo<<"| Current\t";
    for (i=0; i<7; i++)
        if ((i != 3) && (i != 4)){
            Fract_part = 10 * modf(Percent_Currents[i], &Int_part);
            fileo<<"|"<<setw(4)<<setfill(' ')<<Quantity_Currents[i]<<" ("<<setw(3)<<setfill(' ')<<int(Percent_Currents[i])<<"."<<int(Fract_part)<<"%)"<<"\t";
        }
    fileo<<"|"<<endl;
    fileo<<"| Baseline\t";
    for (i=0; i<7; i++)
        if ((i != 3) && (i != 4)){
            Fract_part = 10 * modf(Percent_Baseline[i], &Int_part);
            fileo<<"|"<<setw(4)<<setfill(' ')<<Quantity_Baseline[i]<<" ("<<setw(3)<<setfill(' ')<<int(Percent_Baseline[i])<<"."<<int(Fract_part)<<"%)"<<"\t";
        }
    fileo<<"|"<<endl;
    fileo<<"| PDO\t\t";
    for (i=0; i<7; i++)
        if ((i != 3) && (i != 4)){
            Fract_part = 10 * modf(Percent_PDO[i], &Int_part);
            fileo<<"|"<<setw(4)<<setfill(' ')<<Quantity_PDO[i]<<" ("<<setw(3)<<setfill(' ')<<int(Percent_PDO[i])<<"."<<int(Fract_part)<<"%)"<<"\t";
        }
    fileo<<"|"<<endl;
    fileo<<"| TDO\t\t";
    for (i=0; i<7; i++)
        if ((i != 3) && (i != 4)){
            Fract_part = 10 * modf(Percent_TDO[i], &Int_part);
            fileo<<"|"<<setw(4)<<setfill(' ')<<Quantity_TDO[i]<<" ("<<setw(3)<<setfill(' ')<<int(Percent_TDO[i])<<"."<<int(Fract_part)<<"%)"<<"\t";
        }
    fileo<<"|"<<endl;
    fileo<<"| ART\t\t";
    for (i=0; i<7; i++)
        if ((i != 3) && (i != 4)){
            Fract_part = 10 * modf(Percent_ART[i], &Int_part);
            fileo<<"|"<<setw(4)<<setfill(' ')<<Quantity_ART[i]<<" ("<<setw(3)<<setfill(' ')<<int(Percent_ART[i])<<"."<<int(Fract_part)<<"%)"<<"\t";
        }
    fileo<<"|"<<endl;
    fileo<<"| DDO\t\t";
    for (i=0; i<7; i++)
        if ((i != 3) && (i != 4)){
            Fract_part = 10 * modf(Percent_DDO[i], &Int_part);
            fileo<<"|"<<setw(4)<<setfill(' ')<<Quantity_DDO[i]<<" ("<<setw(3)<<setfill(' ')<<int(Percent_DDO[i])<<"."<<int(Fract_part)<<"%)"<<"\t";
        }
    fileo<<"|"<<endl;
    fileo<<"|-----------------------------------------------------------------------------------------------|"<<endl;

    //Stop of Statistics block
    for (i = 0; i<arrSize; i++){
        if ((arr[i][8] > startdate) && (arr[i][8] < stopdate)){
            k = 4 + neg_or_pos;
            l = 2 + neg_or_pos;
            m = 6 + neg_or_pos;
            if (arr[i][k] == 1 || arr[i][k] == 21 || arr[i][k] == 221){
                roottree->GetEntry(i);
                if (neg_or_pos == 0){   //in case of NEG
                    VDDP_A = VDDP_A_Max_L0;
                    VDD_A = VDD_A_Max_L0;
                    VDDD_A = VDDD_A_Max_L0;
                    VDAD_A = VDAD_A_Max_L0;
                }
                else{                   //in case of POS
                    VDDP_A = VDDP_A_Max_L0POS;
                    VDD_A = VDD_A_Max_L0POS;
                    VDDD_A = VDDD_A_Max_L0POS;
                    VDAD_A = VDAD_A_Max_L0POS;
                }
            
                convert_to_date(date_of_meas, dateTime, 2);
                //Write main info
                fileo<<"***********************************************************"<<endl;
                fileo<<" VMMId "<<arr[i][0]<<" "<<partID<<" ";
                if (arr[i][l] == 0)
                    fileo<<"FAILED in L0 "<<textBig;
                if (arr[i][l] == 1)
                    fileo<<"PASSED in L0 "<<textBig;
                if (arr[i][l] == 2)
                    fileo<<"Perfect in L0 "<<textBig;
                fileo<<" - "<<date_of_meas<<" userID "<<userID<<endl;
                if (arr[i][l] == 0 || arr[i][l] == 1){//In case of failed chip
                    //Write info about currents
                    fileo<<endl<<"Count defective channels = "<<arr[i][m]<<endl<<endl;
                    fileo<<"           VDDP_L0 "<<textSmall<<"|"<<VDDP_A;
                    // Check the cuts for currents
                    if (VDDP_A > 0 && VDDP_A < 0.5)
                        fileo<<" - PASSED"<<endl;
                    else
                        fileo<<" - FAILED"<<endl;
                    fileo<<"            VDD_L0 "<<textSmall<<"|"<<VDD_A;
                    if (VDD_A > 0 && VDD_A < 1)
                        fileo<<" - PASSED"<<endl;
                    else
                        fileo<<" - FAILED"<<endl;
                    fileo<<"           VDDD_L0 "<<textSmall<<"|"<<VDDD_A;
                    if (VDDD_A > 0 && VDDD_A < 0.6)
                        fileo<<" - PASSED"<<endl;
                    else
                        fileo<<" - FAILED"<<endl;
                    fileo<<"           VDAD_L0 "<<textSmall<<"|"<<VDAD_A;
                    if (VDAD_A > 0 && VDAD_A < 0.12)
                        fileo<<" - PASSED"<<endl<<endl;
                    else
                        fileo<<" - FAILED"<<endl<<endl;
                    //Write number of failed channels in Baseline
                    fileo<<"       Baseline L0 "<<textSmall<<"|";
                    if (neg_or_pos == 0){
                       for (j = 0; j<64; j++){
                           if (bLineNeg[j] < 135 || bLineNeg[j] > 195)
                               fileo<< j <<" ";
                       }
                       fileo<<endl<<endl;
                    }
                    else{
                       for (j = 0; j<64; j++){
                           if (bLinePos[j] < 135 || bLinePos[j] > 195)
                               fileo<< j <<" ";
                       }
                       fileo<<endl<<endl;
                    }  
                    //Write number of failed channels in ART
                    fileo<<"            ART L0 "<<textSmall<<"|";
                    //For NEG
                    if (neg_or_pos == 0){
                        for (vi = numberBadCh_ARTneg->begin(); vi != numberBadCh_ARTneg->end(); ++vi){
                            fileo<< *vi <<" ";
                        }
                        fileo<<endl<<endl;
                    }
                    //For POS
                    else{
                        for (vi = numberBadCh_ARTpos->begin(); vi != numberBadCh_ARTpos->end(); ++vi){
                            fileo<< *vi <<" ";
                        }
                        fileo<<endl<<endl;
                    }
                    
                    //Write number of failed channels in PDO
                    fileo<<"       L0 "<<textSmall<<" PDO mean|";
                    for (j = 0; j<64; j++){
                        if (neg_or_pos == 0){
                            meanValue = PDOMEAN_L0NEG[j];
                            entries = PDOEntriesMEAN_L0NEG[j];
                        }
                        else{
                            meanValue = PDOMEAN_L0POS[j];
                            entries = PDOEntriesMEAN_L0POS[j];
                        }
                        if (meanValue < 200 || meanValue > 1022 || entries < 50){
                            fileo<<j<<" ";
                            counter ++;
                        }
                    }
                    if (counter == 0)
                        fileo<<"PASSED";
                    else
                        counter = 0;
                    fileo<<endl;
                    fileo<<"       L0 "<<textSmall<<" TDO mean|";
                    for (j = 0; j<64; j++){
                        if (neg_or_pos == 0){
                            meanValue = TDOMEAN_L0NEG[j];
                            entries = TDOEntriesMEAN_L0NEG[j];
                        }
                        else{
                            meanValue = TDOMEAN_L0POS[j];
                            entries = TDOEntriesMEAN_L0POS[j];
                        }
                        if (meanValue < 32 || meanValue > 254 || entries < 50){
                            fileo<<j<<" ";
                            counter ++;
                        }
                    }
                    if (counter == 0)
                        fileo<<"PASSED";
                    else
                        counter = 0;
                    fileo<<endl;
                    fileo<<"          DDO "<<textSmall<<" mean|";
                    for (j = 0; j<64; j++){
                        if (neg_or_pos == 0){
                            meanValue = MEAN_DDOneg[j];
                        }
                        else{
                            meanValue = MEAN_DDOpos[j];
                        }
                        if (meanValue < 2 || meanValue > 62){
                            fileo<<j<<" ";
                            counter ++;
                        }
                    }
                    if (counter == 0)
                        fileo<<"PASSED";
                    else
                        counter = 0;
                    fileo<<endl;
                    fileo<<endl;
                }
                fileo<<endl;
            }
        }
    }
}
void write_to_Status(int arr[][11], int arrSize, int FR_KR) //neg = 0, pos = 1, FR = 0, KR = 0
{
    int flags_of_written_chips[arrSize];
    int StatTotal_twoPolarities;
    int flag_general_status;
    int startdate, stopdate;
    if (FR_KR == 0){
        startdate = 1462060800; //1 May 2016
        stopdate = 1556668800; //1 May 2019
    }
    else{
        startdate = 1556668800; //1 May 2019
        stopdate = 1651363200; //1 May 2022
    }
    for (i = 0; i<arrSize; i++){
       flags_of_written_chips[i] = 0; // None of chip is written in Statuses DB yet
    }
    fileo<<"C  -  current"<<endl<<"B  -  Baseline"<<endl<<"P  -  PDO"<<endl<<"T  -  TDO"<<endl<<"D  -  DDO"<<endl<<"A  -  ART"<<endl<<endl<<"<+>  -  failed in positive"<<endl;
    fileo<<"<->  -  failed in negative"<<endl<<endl;//<<"10^4 = 10000  -  Perfect (BombNEG+BombPOS)"<<endl<<"10^3 = 1000  -  PerfectNEG (BombNEG +PassedPOS)"<<endl;
    fileo<<"===================================================================================================================================================================="<<endl;
    fileo<<"VMMId\t|Date\t\t|StatusNeg\t|DeadChNeg\t|StatusPos\t|DeadChPos\t|Log_to_DB_neg\t|Log_to_DB_pos\t|GeneralStatus\t|Fail Reason"<<endl;
    fileo<<"===================================================================================================================================================================="<<endl;
    for (i = 0; i<arrSize; i++){
        if ((arr[i][8] > startdate) && (arr[i][8] < stopdate)){
            flag_general_status = 0;
            if (flags_of_written_chips[i] == 0){ //If this chip was not written in the DB
                for (j = 0; j<arrSize; j++){ //Searching again in this array - only chips with this ID
                    if (arr[i][0] == arr[j][0] && flags_of_written_chips[j] == 0){ //and it wasnt writen yet
                        convert_to_date(date_of_meas, arr[j][8], 2);
                        fileo<<arr[j][0]<<"\t|"<<date_of_meas<<"\t|";
                        if (arr[j][2] == 0)
                            fileo<<"Failed\t\t|";
                        if (arr[j][2] == 1)
                            fileo<<"Passed\t\t|";
                        if (arr[j][2] == 2)
                            fileo<<"Bomb\t\t|";
                        fileo<<arr[j][6]<<"\t\t|";
                        if (arr[j][3] == 0)
                            fileo<<"Failed\t\t|";
                        if (arr[j][3] == 1)
                            fileo<<"Passed\t\t|";
                        if (arr[j][3] == 2)
                            fileo<<"Bomb\t\t|";
                        fileo<<arr[j][7]<<"\t\t|";//we will write it in Statuses
                        if (arr[j][4] == 1 || arr[j][4] == 21 || arr[j][4] == 221)
                            fileo<<"yes\t\t|";
                        else
                            fileo<<"no\t\t|";
                        if (arr[j][5] == 1 || arr[j][5] == 21 || arr[j][5] == 221)
                            fileo<<"yes\t\t|";
                        else
                            fileo<<"no\t\t|";
                        StatTotal_twoPolarities = calculate_Total_Stat_two_Polarities(arr, arrSize, arr[i][0]);
                        if (flag_general_status == 0){
                            if (StatTotal_twoPolarities == 4){
                                fileo<<"Perfect\t|";
                                flag_general_status = 1;
                            }
                            if (StatTotal_twoPolarities == 3){
                                fileo<<"PerfectNEG\t|";
                                flag_general_status = 1;
                            }
                            if (StatTotal_twoPolarities == 2){
                                fileo<<"PerfectPOS\t|";
                                flag_general_status = 1;
                            }
                            if (StatTotal_twoPolarities == 1){
                                fileo<<"Passed\t\t|";
                                flag_general_status = 1;
                            }
                            if (StatTotal_twoPolarities == 0){
                                fileo<<"FAILED\t\t|";
                                flag_general_status = 1;
                            }
                        }
                        else
                            fileo<<"\t\t|";
                        roottree->GetEntry(j);
                        fileo<<FAILED<<endl;
                        flags_of_written_chips[j] = 1; //and change the flag as written
                    }
                }
                fileo<<"--------------------------------------------------------------------------------------------------------------------------------------------------------------------"<<endl;
            }
        }
    }
}
void write_header(){
    fileo<<"==============VMMId certification=============="<<endl<<"VMM3a FR 30 18"<<endl<<"VMM3a FR 31 18"<<endl<<"Data taking entries(statistics):"<<endl;
    fileo<<"              Baseline 50"<<endl<<"                   ART 49"<<endl<<"                    L0 99"<<endl<<"                   DDO 99"<<endl;
    fileo<<"=========================================================="<<endl<<"Below selection done with MEAN criteria :"<<endl<<endl<<"Defective channels <= 2"<<endl<<endl;
    fileo<<"L0 and DDO efficient cut > 50(entries)"<<endl<<endl<<"Sum currents L0 negative 0 - 1.7"<<endl<<endl<<"VDDP L0 neg current 0 - 0.5"<<endl;
    fileo<<" VDD L0 neg current 0 - 1"<<endl<<"VDDD L0 neg current 0 - 0.6"<<endl<<"VDAD L0 neg current 0 - 0.12"<<endl<<endl<<"Sum currents L0 positive 0 - 1.7"<<endl<<endl;
    fileo<<"VDDP L0 POS current 0 - 0.5"<<endl<<" VDD L0 POS current 0 - 1"<<endl<<"VDDD L0 POS current 0 - 0.6"<<endl<<"VDAD L0 POS current 0 - 0.12"<<endl<<endl;
    fileo/*<<"           Baseline 135...195"<<endl*/<<"                ART 64 efficient channel"<<endl<<endl<<"    L0 neg PDO mean 200...1022"<<endl;
    fileo<<"    L0 neg TDO mean 32...254"<<endl<<"  DDO negative mean 6...62"<<endl<<endl<<"    L0 pos PDO mean 200...1022"<<endl;
    fileo<<"    L0 pos TDO mean 32...254"<<endl<<"  DDO positive mean 6...62"<<endl<<endl<<endl<<endl;
}

int checkFile(int neg_or_pos){
    string textBig_f;
    //This block is needed to format link in a correct way
    if (VMMId < 1000)
        VMMID_for_link[0] = '0';
    if (VMMId < 100)
        VMMID_for_link[1] = '0';
    if (VMMId < 10)
        VMMID_for_link[2] = '0';
    if (VMMId > 1000){
        VMMID_for_link[0] = '\0';
        VMMID_for_link[1] = '\0';
        VMMID_for_link[2] = '\0';
    }
    if (neg_or_pos == 0)
        textBig_f = "NEG";
    else
        textBig_f = "POS";
    sstm<<"/output/"<<date_of_meas<<"/constants/"<<chipStatus<<"_const_"<<VMMID_for_link<<VMMId<<textBig_f<<".txt";
    file_to_check = sstm.str();
    //if (VMMId == 1202 && neg_or_pos == 1)
    //    cout<<VMMId<<" "<<file_to_check<<endl;
    sstm.str("");
    std::ifstream file(file_to_check);
    if(file.is_open())
        return 1;
    else
        return 0;
}

void one_1Dhist(string xname, string yname, TH1F *hist, int log_or_not){
    int nx = xname.length();
    int ny = yname.length();
    char x[nx+1];
    char y[ny+1];
    strcpy(x, xname.c_str());
    strcpy(y, yname.c_str());
    Canv->Divide(1,1);
    Canv->cd(1);
    if (log_or_not == 1)
        gPad->SetLogy(); //to make a log scale
    hist->Draw("colz");
    hist->SetXTitle(x);
    hist->SetYTitle(y);
    Canv->Print(pdfname, "pdf");
    Canv->Clear();
}

void two_1Dhist(string xname1, string yname1, string xname2, string yname2, TH1F *hist1, TH1F *hist2){
    int nx1 = xname1.length();
    int ny1 = yname1.length();
    char x1[nx1+1];
    char y1[ny1+1];
    int nx2 = xname2.length();
    int ny2 = yname2.length();
    char x2[nx2+1];
    char y2[ny2+1];
    strcpy(x1, xname1.c_str());
    strcpy(y1, yname1.c_str());
    strcpy(x2, xname2.c_str());
    strcpy(y2, yname2.c_str());

    Canv->Divide(1,2);
    Canv->cd(1);
    gPad->SetLogy();
    hist1->Draw("colz");
    hist1->SetXTitle(x1);
    hist1->SetYTitle(y1);

    Canv->cd(2);
    gPad->SetLogy();
    hist2->Draw("colz");
    hist2->SetXTitle(x2);
    hist2->SetYTitle(y2);
    Canv->Print(pdfname, "pdf");
    Canv->Clear();
} 

void one_2Dhist(string xname, string yname, TH2F *hist){
    int nx = xname.length(); 
    int ny = yname.length();
    char x[nx+1]; 
    char y[ny+1];
    strcpy(x, xname.c_str());
    strcpy(y, yname.c_str());
    Canv->Divide(1,1);
    Canv->cd(1);
    hist->Draw("colz");
    hist->SetXTitle(x);
    hist->SetYTitle(y);
    Canv->Print(pdfname, "pdf");
    Canv->Clear();
}

void two_2Dhist(string xname1, string yname1, string xname2, string yname2, TH2F *hist1, TH2F *hist2){
    int nx1 = xname1.length();
    int ny1 = yname1.length();
    char x1[nx1+1];
    char y1[ny1+1];
    int nx2 = xname2.length();
    int ny2 = yname2.length();
    char x2[nx2+1];
    char y2[ny2+1];
    strcpy(y2, yname2.c_str());

    Canv->Divide(1,2);
    Canv->cd(1);
    //gPad->SetLogy();
    hist1->Draw("colz");
    hist1->SetXTitle(x1);
    hist1->SetYTitle(y1);

    Canv->cd(2);
    //gPad->SetLogy();
    hist2->Draw("colz");
    hist2->SetXTitle(x2);
    hist2->SetYTitle(y2);
    Canv->Print(pdfname, "pdf");
    Canv->Clear();
}
