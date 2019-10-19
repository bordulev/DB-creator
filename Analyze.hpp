//
//  Analyze.hpp
//  
//
//  Created by Юрий Бордулев on 28/01/2019.
//

#ifndef Analyze_hpp
#define Analyze_hpp

#include <stdio.h>
#endif /* Analyze_hpp */
#include <iostream> //для использования вывода на консоль
#include <time.h> // Для вывода времени тестирования time_t, struct tm, timem localtime
#include <fstream> // для работы с файлами
#include <typeinfo> //to know the type of the file
#include <sstream> //to convert from int to str and visa versa
#include <vector>
#include <string>
#include <cstring>
#include <ctype.h>
#include <iomanip> // To print the defined number of digits after decimal point
#include <math.h> //To separate integer part from the fractional part of the float number

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h> //To use gStyle
#include <TFile.h>
#include <TTree.h>
//#include <TRandom.h>

using namespace std;

TFile *rootfile;
TTree *roottree;

//for hsitograms
TH2F *hSatus_VS_VMMId = new TH2F("hSatus_VS_VMMId", "Status VS VMMID", 105, 140000, 162000, 7, -1, 7);//0 histogram
TH1F *hbline = new TH1F("hbaseline", "Baseline  ", 300,50,250);//1st and 2nd hist - Baseline
TH2F *hblineVMM = new TH2F("hblineVMM", "Baseline vs VMMId  ", 105,50,250,105,0,3300);//3rd VMMMin, VMMMax
TH2F *hblineTEMP = new TH2F("hblineTEMP", "Baseline vs tempreture  ", 105,50,250,105,30,50);//4th
TH2F *hBlinecor = new TH2F("hBlinecor","Bline pos (delay 2 ms) vs Bline neg (delay 0 ms) - not failed", 300,0,500,300,0,500);//5th
TH2F *hALLCHANNEL = new TH2F("ALLCHANNEL", "PDO mean L0 negative (all VMM)  ", 105, 0, 1023, 105, 0, 65);//6th
TH2F *hPDOEntriesMEANL0 = new TH2F("hPDOEntriesMEANL0", "PDO entries mean L0 negative  ", 105, 1, 120,105,0,65);//7th
TH2F *hALLCHANNELtdol0 = new TH2F("ALLCHANNEL_tdo_L0", "TDO mean L0 negative (all VMM)  ", 105, 0, 260, 105, 0, 65);//8th
TH2F *hTDOEntriesMEANL0 = new TH2F("hTDOEntriesMEANL0", "TDO entries mean L0 negative  ", 105, 1, 120,105,0,65);//9th
TH2F *hALLCHANNELpdol0pos = new TH2F("ALLCHANNEL_pdo_L0POS", "PDO mean L0 positive (all VMM) ", 105, 0, 1023, 105, 0, 65);//10th
TH2F *hPDOEntriesMEANL0POS = new TH2F("hPDOEntriesMEANL0POS", "PDO entries mean L0 positive  ", 105, 1, 120,105,0,65);//11th
TH2F *hALLCHANNELtdol0pos = new TH2F("ALLCHANNEL_tdo_L0POS", "TDO mean L0 positive (all VMM)", 105, 0, 260, 105, 0, 65);//12th
TH2F *hTDOEntriesMEANL0POS = new TH2F("hTDOEntriesMEANL0POS", "TDO entries mean L0 positive  ", 105, 1, 120,105,0,65);//13th
TH2F *hDDOnegneg_ch = new TH2F("DDOnegneg_ch", "DDO mean negative (all VMM) ", 105, 1, 63, 105, 0, 65);//14
TH2F *hDDOpos_ch = new TH2F("DDOpos_ch", "DDO mean positive (all VMM) ", 105, 1, 63, 105, 0, 65);//15
TH1F *hVDD = new TH1F("hVDD", "VDD L0 negative ", 300, 0, 1);//16
TH1F *hVDDpos = new TH1F("hVDDpos", "VDD L0 positive ", 300, 0, 1);//17
TH1F *hVDDP = new TH1F("hVDDP", "VDDP L0 negative ", 150, 0, 0.5);//18
TH1F *hVDDPpos = new TH1F("hVDDPpos", "VDDP L0 positive ", 150, 0, 0.5);//19
TH1F *hVDDD = new TH1F("hVDDD", "VDDD L0 negative ", 300, 0, 0.6);//20
TH1F *hVDDDpos = new TH1F("hVDDDpos", "VDDD L0 positive ", 300, 0, 0.6);//21
TH1F *hVDAD = new TH1F("hVDAD", "VDAD L0 negative ", 300, 0, 0.12);//22
TH1F *hVDADpos = new TH1F("hVDADpos", "VDAD L0 positive ", 300, 0, 0.12);//23
TH2F *htempAVDD = new TH2F("tempAVDD", "Current VDD L0 neg ",105,30,52,105,0,1);//24
TH2F *htempAVDDpos = new TH2F("tempAVDDpos", "Current VDD L0 pos ",105,30,52,105,0,1);//25
TH2F *htempAVDDP = new TH2F("tempAVDDP", "Current VDDP L0 neg ",105,30,52,105,0,0.5);//26
TH2F *htempAVDDPpos = new TH2F("tempAVDDPpos", "Current VDDP L0 pos ",105,30,52,105,0,0.5);//27
TH2F *htempAVDDD = new TH2F("htempAVDDD", "Current VDDD L0 neg ",105,30,52,105,0,0.6);//28
TH2F *htempAVDDDpos = new TH2F("htempAVDDDpos", "Current VDDD L0 pos ",105,30,52,105,0,0.6);//29
TH2F *htempA = new TH2F("htempA", "Current VDAD L0 neg ",105,30,52,105,0,0.12);//30
TH2F *htempApos = new TH2F("htempApos", "Current VDAD L0 pos ",105,30,52,105,0,0.12);//31
TH1F *hsumcurrent_L0 = new TH1F("Sum_current_L0neg","Sum current L0 negative ", 300, 0, 1.7);//32
TH1F *hsumcurrent_L0POS = new TH1F("Sum_current_L0pos","Sum current L0 positive ", 300, 0, 1.7);//33
TH2F *hARTnegNEG =new TH2F("hARTnegNEG","Number of channels with bad ART negative (zero means no defects channels in chip)  ", 105,0,3300,105,1,65);//34 VMMMin, VMMMax
TH2F *hEntriesARTneg = new TH2F("hEntriesARTneg", "ART negative entries  ", 105, 1, 70, 105, 0, 65);//35
TH2F *hARTnegNEG1 =new TH2F("hARTnegNEG1","VMMId with bad ART negative (zero means no defects channels in chip) ", 300,0,3300,300,0,65);//36 VMMMin, VMMMax
TH1F *hARTcountBad = new TH1F("ARTcountBad", "count Bad channel in ART negative", 300, 0, 64);//37
TH2F *hARTpos =new TH2F("hARTpos","Number of channels with bad ART positive (zero means no defects channels in chip)  ", 105,0,3300,105,1,65);//38 VMMMin, VMMMax
TH2F *hEntriesARTpos = new TH2F("hEntriesARTpos", "ART positive entries  ", 105, 1, 70, 105, 0, 65);//39
TH2F *hARTpos2 =new TH2F("hARTpos2","VMMId with bad ART positive (zero means no defects channels in chip) ", 300,0,3300,300,0,65);//40 VMMMin, VMMMax
TH1F *hARTposcountBad = new TH1F("ARTposcountBad", "count Bad channel in ART positive", 300, 0, 64);//41
TH1F *hPDORMS_L0 = new TH1F("hPDORMS_L0", "PDO RMS negative  ", 500, 0, 100);//42
TH1F *hPDORMS_L0POS = new TH1F("hPDORMS_L0POS", "PDO RMS positive  ", 500, 0, 100);//43
TH1F *hblineRMS = new TH1F("hblineRMS", "Baseline RMS  ", 300,0,2);//44
TH1F *hPDOIntercept_L0 = new TH1F("hPDOIntercept_L0","PDO Intercept negative ", 400,-200,800);//45
TH1F *hPDOIntercept_L0POS = new TH1F("hPDOIntercept_L0POS","PDO Intercept positive  ", 400,-200,800);//46
TH1F *hPDOSlope_L0 = new TH1F("hPDOSlope_L0","PDO Slope L0 negative  ", 200, -10,10);//47
TH1F *hPDOSlope_L0POS = new TH1F("hPDOSlope_L0POS","PDO Slope positive  ", 200, -10,10);//48
TH1F *hTDORMS_L0 = new TH1F("hTDORMS_L0", "TDO RMS negative  ", 500, 0, 150);//49
TH1F *hTDORMS_L0POS = new TH1F("hTDORMS_L0POS", "TDO RMS positive  ", 500, 0, 150);//50
TH1F *hTDOIntercept_L0 = new TH1F("hTDOIntercept_L0","TDO Intercept L0 negative  ", 400,-100,400);//51
TH1F *hTDOIntercept_L0POS = new TH1F("hTDOIntercept_L0POS","TDO Intercept positive  ", 400,-100,400);//52
TH1F *hTDOSlope_L0 = new TH1F("hTDOSlope_L0","TDO Slope negative  ", 200, -50,50);//53
TH1F *hTDOSlope_L0POS = new TH1F("hTDOSlope_L0POS","TDO Slope positive  ", 200, -50,50);//54

TCanvas* Canv;
char pdfname[256], pdfopen[256], pdfclose[256];

TString path_to_DB_creator = "/afs/cern.ch/user/i/ibordule/prod/DB_creator";
TString path_to_root = path_to_DB_creator + "/input/const.root";
TString path_to_file_database = path_to_DB_creator + "/output/summary/best_test/Database.txt";
TString path_to_file_GlobalLog = path_to_DB_creator + "/output/summary/log_to_global/globalLog.txt";
TString path_to_file_GlobalLog_dead_ch = path_to_DB_creator + "/output/summary/log_to_global/globalLog_dead_ch.txt";
TString path_to_file_GlobalLog_dead_ch_Bline = path_to_DB_creator + "/output/summary/log_to_global/globalLog_dead_ch_Bline.txt";
TString path_to_file_GlobalLog_dead_ch_ART = path_to_DB_creator + "/output/summary/log_to_global/globalLog_dead_ch_ART.txt";
TString path_to_file_GlobalLog_dead_ch_PDO = path_to_DB_creator + "/output/summary/log_to_global/globalLog_dead_ch_PDO.txt";
TString path_to_file_GlobalLog_dead_ch_TDO = path_to_DB_creator + "/output/summary/log_to_global/globalLog_dead_ch_TDO.txt";
TString path_to_file_GlobalLog_dead_ch_DDO = path_to_DB_creator + "/output/summary/log_to_global/globalLog_dead_ch_DDO.txt";
TString path_to_file_GlobalLog_Bline = path_to_DB_creator + "/output/summary/log_to_global/globalLog_Bline.txt";
TString path_to_file_GlobalLog_PDO_mean = path_to_DB_creator + "/output/summary/log_to_global/globalLog_PDO_mean.txt";
TString path_to_file_GlobalLog_TDO_mean = path_to_DB_creator + "/output/summary/log_to_global/globalLog_TDO_mean.txt";
TString path_to_file_GlobalLog_PDO_entries = path_to_DB_creator + "/output/summary/log_to_global/globalLog_PDO_entries.txt";
TString path_to_file_GlobalLog_TDO_entries = path_to_DB_creator + "/output/summary/log_to_global/globalLog_TDO_entries.txt";
TString path_to_file_GlobalLog_DDO_mean = path_to_DB_creator + "/output/summary/log_to_global/globalLog_DDO_mean.txt";
TString path_to_file_GlobalLog_ART_entries = path_to_DB_creator + "/output/summary/log_to_global/globalLog_ART_entries.txt";

//TString upload_chips = "list.txt";
TString path_to_SN_file = "input/SN.log";
TString path_to_EQ_ID = "input_files/EQ_ID.txt";
TString path_to_file_certificate_FR = path_to_DB_creator + "/output/summary/certification/CERT_FR.txt";
TString path_to_file_certificate_KR = path_to_DB_creator + "/output/summary/certification/CERT_KR.txt";
TString path_to_web_KR  = "https://atlas-tsu.cern.ch/www/output/";
TString path_to_web_FR  = "https://atlas-tsu.cern.ch/www/output/2018/";
TString path_to_file_status_FR = path_to_DB_creator + "/output/summary/status/STATUS_FR.txt";
TString path_to_file_status_KR = path_to_DB_creator + "/output/summary/status/STATUS_KR.txt";
TString path_to_hist_FR = path_to_DB_creator + "/output/summary/histogram/HIST_FR.pdf";
TString path_to_hist_KR = path_to_DB_creator + "/output/summary/histogram/HIST_KR.pdf";
string file_to_check;

Char_t userID[30];//Пользователь
Char_t partID[30];
Char_t FPGA_VER[30];
Char_t FAILED[64];
Char_t date_of_meas[20]; //Date of Measurement
Char_t folderTesting[30]; //Folder, where measurement is stored
Char_t VMMID_for_link[10]; //Chip number to output in the link
string textBig, textSmall, textDefault;//to fill the text in Database.txt


Int_t VMMId; //Номер чипа
Int_t boardId;
Int_t chipStatusNEG; //0, 1, 2
Int_t chipStatusPOS; //0, 1, 2
Int_t chipStatus; //0-6
UInt_t dateTime; //Datetime of the entry (measurement) in uint format
Int_t Total_Status = 0; //Sum of General Statuses for all measurements of a chip
//Currents neg and pos
Float_t VDDP_A_Max_L0;
Float_t VDDP_A_Max_L0POS;
Float_t VDD_A_Max_L0;
Float_t VDD_A_Max_L0POS;
Float_t VDDD_A_Max_L0;
Float_t VDDD_A_Max_L0POS;
Float_t VDAD_A_Max_L0;
Float_t VDAD_A_Max_L0POS;
//Baseline
Float_t bLinePos[64];
Float_t bLineNeg[64];
Float_t bLine_RMS[64];
Float_t bLinePausePos;
//Temperature
Float_t tempMax_L0, tempMax_L0POS;
//PDO and TDO mean value in each channel
Float_t PDOMEAN_L0POS[64];
Float_t PDOMEAN_L0NEG[64];
Float_t TDOMEAN_L0POS[64];
Float_t TDOMEAN_L0NEG[64];
Float_t MEAN_DDOneg[64];
Float_t MEAN_DDOpos[64];
Float_t PDORMS_L0[64];
Float_t PDORMS_L0POS[64]; 
//PDO and TDO fit coefficients
Float_t PDOIntercept_L0[64];
Float_t PDOIntercept_L0POS[64];
Float_t PDOSlope_L0[64];
Float_t PDOSlope_L0POS[64];
Float_t TDORMS_L0[64];
Float_t TDORMS_L0POS[64];
Float_t TDOIntercept_L0[64];
Float_t TDOIntercept_L0POS[64];
Float_t TDOSlope_L0[64];
Float_t TDOSlope_L0POS[64];
//Number of Entries for PDO and TDO in each channel
Int_t PDOEntriesMEAN_L0POS[64];
Int_t PDOEntriesMEAN_L0NEG[64];
Int_t TDOEntriesMEAN_L0POS[64];
Int_t TDOEntriesMEAN_L0NEG[64];
Int_t EntriesARTpos[64];
Int_t EntriesARTneg[64];

Int_t countBadCh_ARTneg;
Int_t countBadCh_ARTpos;

TBranch        *b_VDDP_A_Max_L0; // branch for vector numbers of dead channels for positive
TBranch        *b_pos_DEAD_CHANNELS; // branch for vector numbers of dead channels for positive
TBranch        *b_neg_DEAD_CHANNELS;

vector<int> *pos_DEAD_CHANNELS;
vector<int> *neg_DEAD_CHANNELS; //Vector of numbers of dead channels for positive and negative mode
vector<int> *numberBadCh_ARTneg;
vector<int> *numberBadCh_ARTpos;

Int_t counter = 0;
Int_t counter1 = 0;
Int_t counter2 = 0;


Int_t flag;
Int_t number_of_chips;
Int_t i, j, k, l, m; //default counters in "For" cycles
vector<int>::const_iterator vi; //iterator for the int vector
Int_t size_neg, size_pos;

ofstream fileo, fileo_dead_ch, fileo_dead_ch_Bline, fileo_dead_ch_ART, fileo_dead_ch_PDO, fileo_dead_ch_TDO, fileo_dead_ch_DDO, fileo_Bline, fileo_PDO_mean, fileo_TDO_mean, fileo_PDO_entries, fileo_TDO_entries, fileo_DDO_mean, fileo_ART_entries, outputfile;
stringstream sstm;
const int string_len = 100; //TEMP. to work with Added_SN and EQ_ID txt files 
const int number_of_strings = 1514; //TEMP. to work with Added_SN ans EQ_ID txt files
const char ch_end_of_line = '\n'; //TEMP. to work with Added_SN ans EQ_ID txt files
char mass_EQ_ID[number_of_strings][string_len], mass_Added_SN[number_of_strings][string_len]; //TEMP. to work with Added_SN ans EQ_ID txt files.
int mass_EQID[number_of_strings], mass_AddedSN[number_of_strings]; //TEMP. to work with Added_SN ans EQ_ID txt files.
