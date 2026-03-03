// test.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "test.h"
#include "commdlg.h"
#include "comutil.h"
#include "windows.h"
#include <string>
#include <ctime>

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "EndOfFile")

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

OPENFILENAME ofn;       // структура станд. блока диалога
HANDLE hf;              // дескриптор файла
_bstr_t result = "";
_bstr_t strFileName = "";
_bstr_t strFileNameUsl = "";
_bstr_t strFolderName = "";

std::string ofn_lpstrFile ; // имя файла

// Отправить объявления функций, включенных в этот модуль кода:
//ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInputFile(HINSTANCE, int);

FILE *flog; // 
FILE *fmis; // ошибки по возрасту

inline void TESTHR(HRESULT x) { if FAILED(x) _com_issue_error(x); };

void PrintComError(_com_error &e)
{
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	
	result = " с ошибками.\nИнформацию о них можно найти в файле '" + strFolderName + "forambul.log'.";

	// Print COM errors. 
	fprintf(flog,"error code = %08lx ", e.Error());
	fprintf(flog,"\n  Code meaning = %s ", e.ErrorMessage());
	fprintf(flog,"\n  Source = %s ", (LPCSTR) bstrSource);
	fprintf(flog,"\n  Description = %s ", (LPCSTR) bstrDescription);
}

_bstr_t TrimR(_bstr_t str)
{
	BSTR p = str;
	p = p + str.length(); 
	p--;
	while (*p == ' ') p--; // ищем последний непробел
	p++; // ставим указатель на первый пробел
	*p = 0; // ноль - конец строки
	return str;
}

void Print953000Error(_bstr_t mis)
{
	// Print 953000 errors. 
	fprintf(fmis,"%s,", (LPTSTR) TrimR(mis));
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	MSG msg;
	//HACCEL hAccelTable; // 4 keyboard accelerator

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TEST, szWindowClass, MAX_LOADSTRING);
	//MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInputFile (hInstance, nCmdShow))
	{
		return FALSE;
	}

	result = " успешно.";

	char fdisk[_MAX_DRIVE];
	char fpath[_MAX_DIR];
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];
	char timebuffer[32];
        
	//TCHAR path[MAX_PATH];
	//GetModuleFileName(NULL, path, MAX_PATH);
	//_splitpath_s(path, fdisk, fpath, fname, fext);

	// 4 keyboard accelerator
	//hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEST));

	_splitpath_s(ofn_lpstrFile.c_str() , fdisk, fpath, fname, fext);

	strFolderName += fdisk; 
	strFolderName += fpath; 
	strFolderName += "forambul.log"; 
    //flog = fopen((LPTSTR) strFolderName, "a+");
    fopen_s(&flog,(LPTSTR) strFolderName, "a+");

	strFolderName = "";
	strFolderName += fdisk; 
	strFolderName += fpath; 
	strFolderName += "953000.csv"; 
	//fmis = fopen((LPTSTR)strFolderName, "a+");
	fopen_s(&fmis,(LPTSTR)strFolderName, "a+");

	strFolderName = "";
	strFileName += fname;
	strFolderName += fdisk;
	strFolderName += fpath;
	
	strFileNameUsl += fname;
	const char *_patt_test = strstr((LPCTSTR)strFileNameUsl,"pat-");
	if ( _patt_test == NULL ) {
		std::string f_usl =(LPCTSTR) strFileNameUsl;
		f_usl.replace(0, 3, "pat_usl"); 
		strFileNameUsl = _com_util::ConvertStringToBSTR(f_usl.c_str());
	} else {
		strFileNameUsl += "_usl";
	}
	fprintf(flog,strFileNameUsl);

		
	CoInitialize(NULL);
	_ConnectionPtr cnn=NULL;
	
	_bstr_t strConn;

	HRESULT hr_cn;
    try {
 		time_t seconds = time(NULL);
		struct tm timeinfo;
		localtime_s(&timeinfo,&seconds);

		asctime_s(timebuffer,32,&timeinfo);
		fprintf(flog,"\n\n---------------------------------------------\n%s", timebuffer);
		fprintf(flog,"Microsoft Visual FoxPro Driver: ");
		strConn = "Provider=MSDASQL.1;Driver={Microsoft Visual FoxPro Driver};UID=;PWD=;SourceDB=" + strFolderName + ";SourceType=DBF;Exclusive=Yes;BackgroundFetch=No;Collate=Machine;";
		hr_cn = cnn.CreateInstance(__uuidof(Connection));
		cnn->Open(strConn,"","",adConnectUnspecified);
		fprintf(flog,"OK");
		result = " успешно.";
	}
	catch (_com_error e){
		PrintComError(e);
		try {
			time_t seconds = time(NULL);
			struct tm timeinfo;
			localtime_s(&timeinfo,&seconds);

			asctime_s(timebuffer,32,&timeinfo);
			fprintf(flog,(char *)"\n\n---------------------------------------------\n%s", timebuffer);
 			fprintf(flog,"Microsoft OLE DB Provider for Visual FoxPro: ");
			strConn = "Provider='vfpoledb.1'; Data Source='" + strFolderName + "'; SourceType=DBF;";
			cnn->Open(strConn,"","",adConnectUnspecified);
			fprintf(flog,"OK");
			result = " успешно.";
		}
		catch (_com_error e){
			PrintComError(e);
		}
	}


	bool toCount = false;

	_bstr_t m_cRec = "";
	/* da2 уже сам делает
	try {
		m_cRec = "ALTER TABLE " + strFileName + " ADD COLUMN VID_TR N(1,0)"; fprintf(flog,"\n ADD COLUMN VID_TR: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE " + strFileName + " ADD COLUMN DOC_OMS N(0,0)"; fprintf(flog,"\n ADD COLUMN DOC_OMS: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE " + strFileName + " ADD COLUMN POS_03 N(2,0)"; fprintf(flog,"\n ADD COLUMN POS_03: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE " + strFileName + " ADD COLUMN AIM N(0,0)"; fprintf(flog,"\n ADD COLUMN AIM: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE " + strFileName + " ADD COLUMN ID C(20)"; fprintf(flog,"\n ADD COLUMN ID: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE " + strFileName + " ADD COLUMN LGOTA N(0,0)"; fprintf(flog,"\n ADD COLUMN LGOTA: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");		
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE " + strFileName + " ADD COLUMN FOR_POM N(0,0)"; fprintf(flog,"\n ADD COLUMN FOR_POM: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");		
		toCount = true; // это - выгрузка из дантиста
	}
	catch (_com_error e){
		PrintComError(e);
	}
	*/

	// для Ambul v_1.0.14.2	
	// PROFK,N,2,0	METHMP,N,5,1	VIZ_1,C,5	VIZ_2,C,5	OT_PERV,C,2	
	// POS_NEOTL,N,3,0	PROFIL,N,3,0	NOMKID,C,20	KSG,C,5	MES2,C,6	DATE_A,D	DATE_NEOTL,D
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN DATE_NEOTL D"; fprintf(flog,"\n ADD COLUMN DATE_NEOTL: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN DATE_A D"; fprintf(flog,"\n ADD COLUMN DATE_A: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN MES2 C(6)"; fprintf(flog,"\n ADD COLUMN MES2: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN KSG C(5)"; fprintf(flog,"\n ADD COLUMN KSG: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN NOMKID C(20)"; fprintf(flog,"\n ADD COLUMN NOMKID: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN PROFIL N(2,0)"; fprintf(flog,"\n ADD COLUMN PROFIL: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN POS_NEOTL N(2,0)"; fprintf(flog,"\n ADD COLUMN POS_NEOTL: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN OT_PERV C(2)"; fprintf(flog,"\n ADD COLUMN OT_PERV: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN VIZ_1 C(5)"; fprintf(flog,"\n ADD COLUMN VIZ_1: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN VIZ_2 C(5)"; fprintf(flog,"\n ADD COLUMN VIZ_2: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN METHMP N(4,1)"; fprintf(flog,"\n ADD COLUMN METHMP: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN PROFK N(1,0)"; fprintf(flog,"\n ADD COLUMN PROFK: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}

    // для Ambul v_1.0.16.120	
	// MOP,C,3	LPU_F033,C,17	PROFIL_M,N,2,0	
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN MOP C(3)"; fprintf(flog,"\n ADD COLUMN MOP: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN LPU_F033 C(17)"; fprintf(flog,"\n ADD COLUMN LPU_F033: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileName + "] ADD COLUMN PROFIL_M N(1,0)"; fprintf(flog,"\n ADD COLUMN PROFIL_M: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileNameUsl + "] ADD COLUMN LPU_F033 C(17)"; fprintf(flog,"\n ADD COLUMN LPU_F033: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		m_cRec = "ALTER TABLE [" + strFileNameUsl + "] ADD COLUMN PROFIL_M N(1,0)"; fprintf(flog,"\n ADD COLUMN PROFIL_M: "); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		//  LPU_F033 Уникальный номер структурного подразделения МО по данным ЕРМО. Справочник F033(ГБУЗ НСО "ДГКСП") = 54202601400005012
		//	PROFIL_M Профиль медицинской помощи в соответствии со справочником M003 = 36
		m_cRec = "UPDATE [" + strFileName + "]  SET PROFIL_M=36"; cnn->Execute(m_cRec,NULL,1); 
		m_cRec = "UPDATE [" + strFileName + "]  SET LPU_F033='54202601400002011' WHERE LEFT(AMBKARTA,1)='1'"; cnn->Execute(m_cRec,NULL,1);
		m_cRec = "UPDATE [" + strFileName + "]  SET LPU_F033='54202601400003082' WHERE LEFT(AMBKARTA,1)='2'"; cnn->Execute(m_cRec,NULL,1);
		m_cRec = "UPDATE [" + strFileName + "]  SET LPU_F033='54202601400004066' WHERE LEFT(AMBKARTA,1)='3'"; cnn->Execute(m_cRec,NULL,1);
		m_cRec = "UPDATE [" + strFileName + "]  SET LPU_F033='54202601400004066' WHERE LEFT(AMBKARTA,1)='0'"; cnn->Execute(m_cRec,NULL,1);
		m_cRec = "UPDATE [" + strFileNameUsl + "]  SET PROFIL_M=36"; cnn->Execute(m_cRec,NULL,1); 
		m_cRec = "UPDATE [" + strFileNameUsl + "]  SET LPU_F033='54202601400005012'"; cnn->Execute(m_cRec,NULL,1);
	}
	catch (_com_error e){
		PrintComError(e);
	}
	try {
		//	MOP Место обращения (посещения) Справочник V040 = 1
		m_cRec = "UPDATE [" + strFileName + "]  SET MOP='1'"; cnn->Execute(m_cRec,NULL,1);
	}
	catch (_com_error e){
		PrintComError(e);
	}
	// для Ambul v_1.0.16.120


	try {
		// Форма помощи:
		// 2 - неотложная
		// 3 - плановая
		m_cRec = "UPDATE [" + strFileName + "]  SET FOR_POM=2 WHERE AIM=3"; cnn->Execute(m_cRec,NULL,1); 
		m_cRec = "UPDATE [" + strFileName + "]  SET FOR_POM=3 WHERE !((FOR_POM=2) AND (AIM=3))"; cnn->Execute(m_cRec,NULL,1); 
//m_cRec = "UPDATE " + strFileName + "  SET DOC_OMS='0'"; cnn->Execute(m_cRec,NULL,1); надо разобраться
		m_cRec = "UPDATE [" + strFileName + "]  SET VID_TR='0'"; cnn->Execute(m_cRec,NULL,1);

		// Цель:
		// 2 - профилактическая
		// 3 - неотложная
		// 5 - другое
		m_cRec = "UPDATE [" + strFileName + "]  SET AIM=5 WHERE !((FOR_POM=2) AND (AIM=3))"; cnn->Execute(m_cRec,NULL,1); 
		m_cRec = "UPDATE [" + strFileName + "]  SET AIM=2 WHERE (MES='350103') OR (MES='350109') OR (MES='350113') OR (MES='350116')"; cnn->Execute(m_cRec,NULL,1);
		m_cRec = "UPDATE [" + strFileName + "]  SET AIM=2 WHERE (MES='350120') OR (MES='350121') OR (MES='350122') OR (MES='350123')"; cnn->Execute(m_cRec,NULL,1);
		m_cRec = "UPDATE [" + strFileName + "]  SET AIM=2 WHERE (MES='350124') OR (MES='350125') OR (MES='350126')"; cnn->Execute(m_cRec,NULL,1);
		
		m_cRec = "UPDATE [" + strFileName + "]  SET REZOBR='15' WHERE (MES='350103') OR (MES='350109') OR (MES='350113') OR (MES='350116')"; cnn->Execute(m_cRec,NULL,1);
		m_cRec = "UPDATE [" + strFileName + "]  SET REZOBR='15' WHERE (MES='350120') OR (MES='350121') OR (MES='350122') OR (MES='350123')"; cnn->Execute(m_cRec,NULL,1);
		m_cRec = "UPDATE [" + strFileName + "]  SET REZOBR='15' WHERE (MES='350124') OR (MES='350125') OR (MES='350126')"; cnn->Execute(m_cRec,NULL,1);

		m_cRec = "UPDATE [" + strFileName + "]  SET POS_UET1=0 WHERE (MES='350103') OR (MES='350109') OR (MES='350113') OR (MES='350116')"; cnn->Execute(m_cRec,NULL,1);
		m_cRec = "UPDATE [" + strFileName + "]  SET POS_UET1=0 WHERE (MES='350120') OR (MES='350121') OR (MES='350122') OR (MES='350123')"; cnn->Execute(m_cRec,NULL,1);
		m_cRec = "UPDATE [" + strFileName + "]  SET POS_UET1=0 WHERE (MES='350124') OR (MES='350125') OR (MES='350126')"; cnn->Execute(m_cRec,NULL,1);

	}
	catch (_com_error e){
		PrintComError(e);
	}

	//// Проверка возраста при осмотрах
	//// Вывод - в "953000.txt"
	_RecordsetPtr pAns = NULL;
	_bstr_t p_Val = "";
	setlocale (LC_ALL,"");
	time_t seconds = time(NULL);
	//tm* timeinfo = localtime(&seconds);
	struct tm timeinfo;
	localtime_s(&timeinfo,&seconds);
	asctime_s(timebuffer,32,&timeinfo);
	fprintf(fmis,"\n\n------------------------------\n%s", timebuffer);
	fprintf(fmis,"\nАмб. карта,Пациент,Дата рожд.,МЭС,Леч. окончено,\n");
	try {
		int res = 0;
		m_cRec = "SELECT AMBKARTA, FIO, DR, MES, DAT_BEG, DAT_END FROM [" + strFileName + "] WHERE (MES='953001') OR (MES='953002') OR (MES='953003')"; 
		pAns = cnn->Execute(m_cRec,NULL,1);
		while(!(pAns->EndOfFile)) {
			int lY, lM, lD, rY, rM, rD, Lmin, Lmax, dr, minL, maxL;

			p_Val = pAns->Fields->GetItem("DAT_END")->Value;
			char *buf = (LPTSTR) p_Val ;
			sscanf_s(buf, "%d.%d.%d", &lD, &lM, &lY);

			p_Val = pAns->Fields->GetItem("DR")->Value;
			buf = (LPTSTR) p_Val ;
			sscanf_s(buf, "%d.%d.%d", &rD, &rM, &rY);

			dr = rY*10000 + rM*100 + rD;
			p_Val = pAns->Fields->GetItem("MES")->Value;
			buf = (LPTSTR) p_Val ;
			if (strcmp(buf, "953001")==0) {
				maxL = 3;
				minL = 0;
			} else if (strcmp(buf, "953002")==0) {
				maxL = 15;
				minL = 3;
			} else if (strcmp(buf, "953003")==0) {
				maxL = 18;
				minL = 15;
			}
			Lmin = (lY-maxL)*10000 + lM*100 + lD;
			Lmax = (lY-minL)*10000 + lM*100 + lD;
			
			if ((Lmin >= dr) || (Lmax < dr)) {
				res++;
				Print953000Error(p_Val = pAns->Fields->GetItem("AMBKARTA")->Value); 
				Print953000Error(p_Val = pAns->Fields->GetItem("FIO")->Value); 
				//fprintf(fmis," д.р. ");
				Print953000Error(p_Val = pAns->Fields->GetItem("DR")->Value); 
				Print953000Error(p_Val = pAns->Fields->GetItem("MES")->Value); 
				//fprintf(fmis," леч. ");
				Print953000Error(p_Val = pAns->Fields->GetItem("DAT_END")->Value); 
				fprintf(fmis,"\n");
			}
			pAns ->MoveNext(); 
		}
		pAns->Close(); 
		if (res > 0) result += "\n\nОшибки МЭС 95300х (" + (_bstr_t)res + " шт.) - в файле '" + strFolderName + "953000.csv'."; 
	}
	catch (_com_error e){
		PrintComError(e);
	}

/*	da2 уже сам делает
	// создание таблицы _usl
	// toCount == true - выгрузка из дантиста
	//try {
	//	m_cRec = "SELECT * FROM " + strFileNameUsl; fprintf(flog,"\n " + strFileNameUsl + " уже существует?"); cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");
	//}
	//catch (_com_error e){
	if (toCount == false) {
		try {
			_RecordsetPtr pBns = NULL;
			m_cRec = "CREATE TABLE " + strFileNameUsl + " (ID C(20),AMBKARTA C(10),IDSERV C(36),LPU C(6),LPU_1 C(6),PODR N(7,0),PROFIL N(2,0),DET N(0,0),DATE_IN D,DATE_OUT D,DS C(10),CODE_USL C(16),KOL_USL N(4,2),TARIF N(13,2),SUMV_USL N(13,2),PRVS N(8,0),CODE_MD C(16),COMENTU C(250))"; 
			fprintf(flog,"\n CREATE " + strFileNameUsl); 
			cnn->Execute(m_cRec,NULL,1); fprintf(flog," OK");

			//Нумерация талонов основной таблицы.
			setlocale (LC_ALL,"");
			int res = 0;
			int countID = 100000;
			m_cRec = "SELECT * FROM " + strFileName; // + " WHERE ID='' OR ID IS NULL"; 
			TESTHR(pBns.CreateInstance(__uuidof(Recordset)));
			pBns->CursorType = adOpenKeyset;
			pBns->LockType = adLockOptimistic;
			pBns = cnn->Execute(m_cRec,NULL,1);
			//pBns->Open(m_cRec, strConn, adOpenKeyset, adLockOptimistic, adCmdText);
								
			long em = 0;

			//pBns = cnn->Execute(m_cRec,NULL,1);
			while(!(pBns->EndOfFile)) {
//int rowCount = 0;
//while(rowCount < 100) {
				int dbD, dbM, dbY, deY, deM, deD, diff;
				_bstr_t id="";
				_bstr_t lpu="";
				_bstr_t d_i="";
				_bstr_t d_o="";
				int cu = 1000;
				char *buf;

////while(!(pBns->EndOfFile)) {
//pBns->MoveLast(); 
//pBns->Fields->GetItem("ID")->Value = (_bstr_t)(++rowCount);
////}
//pBns->Update();
//pBns->Close();
			

//if (toCount==true) {
//m_cRec = "UPDATE " + strFileName + "  SET ID='" + (_bstr_t)++rowCount + "'"; //WHERE (MES='350124') OR (MES='350125') OR (MES='350126')"; 
//cnn->Execute(m_cRec,NULL,1);
//pBns->Fields->GetItem("ID")->Value = (_bstr_t)(++rowCount);
//pBns->Update();
//}
				p_Val = (_bstr_t)pBns->Fields->GetItem("ID")->Value;
				id = (LPTSTR) p_Val ;	

				p_Val = pBns->Fields->GetItem("LPU")->Value;
				lpu = (LPTSTR) p_Val ;				
				
				p_Val = pBns->Fields->GetItem("DAT_BEG")->Value;
				buf = (LPTSTR) p_Val ;
				sscanf_s(buf, "%d.%d.%d", &dbD, &dbM, &dbY);
				d_i = "DATE(" + (_bstr_t)dbY + "," + (_bstr_t)dbM + "," + (_bstr_t)dbD + ")";

				p_Val = pBns->Fields->GetItem("DAT_END")->Value;
				buf = (LPTSTR) p_Val ;
				sscanf_s(buf, "%d.%d.%d", &deD, &deM, &deY);
				d_o = "DATE(" + (_bstr_t)deY + "," + (_bstr_t)deM + "," + (_bstr_t)deD + ")";

//em = pBns->EditMode;
//em = pBns->EditMode;
//pBns->Update();
//em = pBns->EditMode;
//pBns->Fields->Refresh();
//p_Val = pBns->Fields->Item->;
				struct tm DAT;
				time_t DAT_BEG, DAT_END;

				DAT.tm_year = dbY - 1900;
				DAT.tm_mon = dbM - 1;
				DAT.tm_mday = dbD;
				DAT.tm_hour = 0; DAT.tm_min = 0; DAT.tm_sec = 0;
				DAT_BEG = mktime(&DAT);

				DAT.tm_year = deY - 1900;
				DAT.tm_mon = deM - 1;
				DAT.tm_mday = deD;
				DAT.tm_hour = 0; DAT.tm_min = 0; DAT.tm_sec = 0;
				DAT_END = mktime(&DAT);

				diff = (int)(difftime(DAT_END, DAT_BEG)/86400);
		
				m_cRec = "INSERT INTO " + strFileNameUsl + "(ID,LPU_1,DATE_IN,DATE_OUT,CODE_USL) VALUES ('" + id + "','" + lpu + "'," + d_i + "," + d_i + ",'" + (_bstr_t)cu + "')"; 
				cnn->Execute(m_cRec,NULL,1);	

				if (diff > 0) {
					m_cRec = "INSERT INTO " + strFileNameUsl + "(ID,LPU_1,DATE_IN,DATE_OUT,CODE_USL) VALUES ('" + id + "','" + lpu + "'," + d_o + "," + d_o + ",'" + (_bstr_t)(cu+diff) + "')"; 
					cnn->Execute(m_cRec,NULL,1);
				}
			
				pBns->MoveNext(); 
			}
			pBns->Close();
		}
		catch (_com_error e){
			MessageBox(NULL,"Добавление дат приёмов не выполнено: необходимо удалить стаарую таблицу '_usl'","forAmbul",MB_OK);
			PrintComError(e);
		}
	}
*/		
	
    fclose(flog);
	fclose(fmis);
	CoUninitialize();
	cnn->Close();
	cnn=NULL;
	MessageBox(NULL,"Обработка '" + strFileName + fext + "' завершена" + result,"forAmbul",MB_OK);

	// 4 keyboard accelerator
	//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
	//{
		//TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}
	
	return 0;//(int) msg.wParam;
}


//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
//  КОММЕНТАРИИ:
//
//    Эта функция и ее использование необходимы только в случае, если нужно, чтобы данный код
//    был совместим с системами Win32, не имеющими функции RegisterClassEx'
//    которая была добавлена в Windows 95. Вызов этой функции важен для того,
//    чтобы приложение получило "качественные" мелкие значки и установило связь
//    с ними.
//
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
//	WNDCLASSEX wcex;
//
//	wcex.cbSize = sizeof(WNDCLASSEX);
//
//	wcex.style			= CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc	= NULL; //WndProc;
//	wcex.cbClsExtra		= 0;
//	wcex.cbWndExtra		= 0;
//	wcex.hInstance		= hInstance;
//	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST));
//	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
//	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TEST);
//	wcex.lpszClassName	= szWindowClass;
//	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
//
//	return RegisterClassEx(&wcex);
//}


BOOL InitInputFile(HINSTANCE hInputFile, int nCmdShow)
{
   //HWND hWnd;

	hInst = hInputFile; // Сохранить дескриптор экземпляра в глобальной переменнойCW_USEDEFAULT

	std::string initDir="";
	int Cd = GetFileAttributes("c:\\chisel");
	if ((Cd != -1) && (FILE_ATTRIBUTE_DIRECTORY & Cd)) {
		initDir="c:\\chisel";
	}
	else {
		initDir="c:\\";
	}

	char szFile[260]="";       // буфер для имени файла

	// Инициализация структуры OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "dBase\0*.dbf\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = initDir.c_str();
	ofn.lpstrTitle = "Выбор файла выгрузки PAT_DA.dbf";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	
	// Показываем на экране диалоговое окно Открыть (Open).
	if (GetOpenFileName(&ofn)==TRUE) {
		//hf = CreateFile(ofn.lpstrFile, GENERIC_READ,
		//	0, (LPSECURITY_ATTRIBUTES) NULL,
		//	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		//	(HANDLE) NULL);
		ofn_lpstrFile =  ofn.lpstrFile;
	   return TRUE;
	}

   return FALSE;
}

