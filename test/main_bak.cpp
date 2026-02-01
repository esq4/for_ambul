// test.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "test.h"
#include "commdlg.h"
#include "windows.h"
#include <string>
#include <stdlib.h>
#include <ctime>
//#include <iostream>
//#include <conio.h>

//#import "msado15.dll" rename_namespace("ado20") rename("EOF", "EndOfFile")
//using namespace ado20;

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "EndOfFile")

//using namespace std;

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

OPENFILENAME ofn;       // структура станд. блока диалога
HANDLE hf;              // дескриптор файла

std::string ofn_lpstrFile ; // имя файла
//char* ofn_lpstrFile ; // имя файла

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void PrintComError(_com_error &e)
{
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());

	FILE *ferr;
	ferr = fopen("forambul.log", "a+");

	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);

	// Print COM errors. 
	fprintf(ferr,"\n\nError: %s", asctime(timeinfo));
	fprintf(ferr,"  Code = %08lx ", e.Error());
	fprintf(ferr,"\n  Code meaning = %s ", e.ErrorMessage());
	fprintf(ferr,"\n  Source = %s ", (LPCSTR) bstrSource);
	fprintf(ferr,"\n  Description = %s ", (LPCSTR) bstrDescription);
	fclose(ferr);
}

//void PrintProviderError(_ConnectionPtr pConnection)
//	{
//	// Print Provider Errors from Connection object.
//	// pErr is a record object in the Connection's Error collection.
//	ErrorPtr pErr = NULL;
//
//	if( (pConnection->Errors->Count) > 0) {
//		long nCount = pConnection->Errors->Count;
//		// Collection ranges from 0 to nCount -1.
//		for(long i = 0;i < nCount;i++) {
//			pErr = pConnection->Errors->GetItem(i);
//			printf(" Error number: %x %s", pErr->Number,
//			pErr->Description); 
//		}
//	}
// }



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	FILE *flog;
	flog = fopen("forambul.log", "a+");

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEST));

//ofn_lpstrFile =  ofn.lpstrFile;std::string
	char fdisk[_MAX_DRIVE];
	char fpath[_MAX_DIR];
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];
	std::string strFileName;
	std::string strFolderName;

	_splitpath_s(ofn_lpstrFile.c_str() , fdisk, fpath, fname, fext);
	strFileName.append(fname);
	//strFileName.append(fext);
	strFolderName.append(fdisk);
	strFolderName.append(fpath);

ofn_lpstrFile =  fdisk;

	CoInitialize(NULL);
	_ConnectionPtr cnn=NULL;

	HRESULT hr_cn;
    try {
		hr_cn = cnn.CreateInstance(__uuidof(Connection));
		_bstr_t strConn = "Provider='vfpoledb.1'; Data Source='C:\\chisel'; SourceType=DBF;";

		fprintf(flog,"  1\n");
 		cnn->Open(strConn,"","",adConnectUnspecified);
		time_t seconds = time(NULL);
		tm* timeinfo = localtime(&seconds);
		fprintf(flog,"\n\n%s", asctime(timeinfo));
		fprintf(flog,"Microsoft OLE DB Provider for Visual FoxPro: OK");
	}
	catch (_com_error e){
		try {
			_bstr_t strConn = "Provider=MSDASQL.1;Driver={Microsoft Visual FoxPro Driver};UID=;PWD=;SourceDB=C:\\chisel;SourceType=DBF;Exclusive=Yes;BackgroundFetch=No;Collate=Machine;";

			printf("\n  2\n");
 			cnn->Open(strConn,"","",adConnectUnspecified);
			time_t seconds = time(NULL);
			tm* timeinfo = localtime(&seconds);
			fprintf(flog,"\n\n%s", asctime(timeinfo));
			fprintf(flog,"Microsoft Visual FoxPro Driver: OK");
		}
		catch (_com_error e){
			PrintComError(e);
			//PrintProviderError(cnn);
		}
	}

	_bstr_t m_cRec = "ALTER TABLE pat_da ADD COLUMN FOR_POM6 N(0,0)";

	try {
		cnn->Execute(m_cRec,NULL,1);
	}
	catch (_com_error e){
		PrintComError(e);
	}


	//_RecordsetPtr rs;

	//_RecordsetPtr rs = NULL;
	//_ConnectionPtr cnn("ADODB.Connection");
	//_CommandPtr cmd("ADODB.Command");

 //   std::string connstr = "Driver={Microsoft Visual FoxPro Driver};UID=;PWD=;SourceDB=";
 //   connstr.append(strFolderName.c_str());
 //   connstr.append(";SourceType=DBF;Exclusive=Yes;BackgroundFetch=No;Collate=Machine;");
 //   //cnn.ConnectionString = connstr;
 //   try {
	//cnn->Open(connstr.c_str(),"","",adConnectUnspecified);
	//}
	//catch (_com_error &e){
	//	connstr.clear();
	//	//cnn->Provider = "Microsoft OLE DB Provider for Visual FoxPro" ;
	//	//cnn->Properties=strcat("Data Source=", strFolderName.c_str());
	//	//cnn->Open(strcat("Data Source=", strFolderName.c_str()),"","",adConnectUnspecified);;Initial Catalog=''yyyyyyydb'
	//std::string strCnn = "Provider=Microsoft OLE DB Provider for Visual FoxPro;Data Source=";
	//strCnn.append(strFolderName.c_str());
	//strCnn.append(";Persist Security Info=True;User ID=;Password=");
 //   //std::string q = "UPDATE [";
 //   //q.append(strFileName.c_str());
 //   //q.append("] SET REZOBR='15' WHERE (MES='350103') OR (MES='350109') OR (MES='350113') OR (MES='350116')");
 //   std::string q = "SELECT * FROM [";
 //   q.append(strFileName.c_str());
 //   q.append("]");
	//strCnn = "Driver={Microsoft Visual FoxPro Driver};UID=;SourceDB=C:\\chisel;SourceType=DBF;Exclusive=Yes;BackgroundFetch=No;Collate=Machine;";
	////cnn->Open(L"Driver={ODBC};UID=;SourceDB=C:\\chisel;SourceType=DBF;Exclusive=Yes;BackgroundFetch=No;Collate=Machine;", "", "", adConnectUnspecified);
	//cnn->Open("Driver={Microsoft OLE DB Provider for Visual FoxPro};UID=;SourceDB=C:\\chisel;SourceType=DBF;Exclusive=Yes;BackgroundFetch=No;Collate=Machine;", "", "", adConnectUnspecified);
	////rs->Open("SELECT * FROM Pat_da", "Driver={Microsoft OLE DB Provider for Visual FoxPro};UID=;SourceDB=C:\chisel;SourceType=DBF;Exclusive=Yes;BackgroundFetch=No;Collate=Machine;", adOpenStatic, adLockReadOnly, adCmdText);
	//}


        //hr = pConn.CreateInstance(__uuidof(Connection));
        //hr = rcd.CreateInstance(__uuidof(Recordset));
        //hr=pConn->Open(_T("Provider='Microsoft.Jet.OLEDB.4.0';Data Source='1.mdb'"),"","",adConnectUnspecified);

		//        Dim rs
//        rs = CreateObject("ADODB.Recordset")
//
//        Dim cnn As Object              //ADODB.Connection
//        Dim connstr As String
//        cnn = CreateObject("ADODB.Connection")
//        connstr = "Driver={Microsoft Visual FoxPro Driver};UID=;PWD=;SourceDB="
//        connstr = connstr & strFolderName
//        connstr = connstr & ";SourceType=DBF;Exclusive=Yes;BackgroundFetch=No;Collate=Machine;"
//        cnn.ConnectionString = connstr
//cont_1:
//        Try
//            cnn.Open()
//        Catch
//            GoTo err_1
//        End Try
//
//        Try
//            rs.Open("ALTER TABLE [" & strFileName & "] ADD COLUMN FOR_POM N(0,0)", cnn)
//            rs.Open("ALTER TABLE [" & strFileName & "] ADD COLUMN AIM N(0,0)", cnn)
//            rs.Open("UPDATE [" & strFileName & "] SET FOR_POM=3", cnn)
//
//            rs.Open("UPDATE [" & strFileName & "] SET AIM=5", cnn)
//            rs.Open("UPDATE [" & strFileName & "] SET AIM=2 WHERE (MES='350103') OR (MES='350109') OR (MES='350113') OR (MES='350116')", cnn)
//            rs.Open("UPDATE [" & strFileName & "] SET AIM=2 WHERE (MES='350120') OR (MES='350121') OR (MES='350122') OR (MES='350123')", cnn)
//            rs.Open("UPDATE [" & strFileName & "] SET AIM=2 WHERE (MES='350124') OR (MES='350125') OR (MES='350126')", cnn)
//
//            rs.Open("UPDATE [" & strFileName & "] SET REZOBR='15' WHERE (MES='350103') OR (MES='350109') OR (MES='350113') OR (MES='350116')", cnn)
//            rs.Open("UPDATE [" & strFileName & "] SET REZOBR='15' WHERE (MES='350120') OR (MES='350121') OR (MES='350122') OR (MES='350123')", cnn)
//            rs.Open("UPDATE [" & strFileName & "] SET REZOBR='15' WHERE (MES='350124') OR (MES='350125') OR (MES='350126')", cnn)
//
//            rs.Open("UPDATE [" & strFileName & "] SET POS_UET1=0 WHERE (MES='350103') OR (MES='350109') OR (MES='350113') OR (MES='350116')", cnn)
//            rs.Open("UPDATE [" & strFileName & "] SET POS_UET1=0 WHERE (MES='350120') OR (MES='350121') OR (MES='350122') OR (MES='350123')", cnn)
//            rs.Open("UPDATE [" & strFileName & "] SET POS_UET1=0 WHERE (MES='350124') OR (MES='350125') OR (MES='350126')", cnn)
//
//            cnn.Close()
//            MsgBox("Обработка " & fileToOpen & " завершена")
//            Exit Sub
//        Catch
//            GoTo err_2
//        End Try
//
//err_2:
//        cnn.Close()
//        MsgBox("Выбраный " & fileToOpen & " уже обработан или не является файлом выгрузки из Dentist")
//        Exit Sub
//
//err_1:
//        cnn = CreateObject("ADODB.Connection")
//        cnn.Provider = "Microsoft OLE DB Provider for Visual FoxPro" 'или "vfpoledb",  "vfpoledb.1"
//        cnn.Properties("Data Source") = strFolderName
//        GoTo cont_1


        //'    With cn
        //'        .ConnectionString = qstr
        //'        .Open
        //'    End With
        //'    'rs.Open "ALTER TABLE [" & strFileName & "] DROP COLUMN _NullFlags", cn
        //'
        //'    cn.Close














	GetMessage(&msg, NULL, 0, 0);
	if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	fclose(flog);
	CoUninitialize();
	cnn->Close();
	cnn=NULL;
	return (int) msg.wParam;
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
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   //HWND hWnd;

	hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменнойCW_USEDEFAULT

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
		hf = CreateFile(ofn.lpstrFile, GENERIC_READ,
			0, (LPSECURITY_ATTRIBUTES) NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
			(HANDLE) NULL);
		ofn_lpstrFile =  ofn.lpstrFile;
	}

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND	- обработка меню приложения
//  WM_PAINT	-Закрасить главное окно
//  WM_DESTROY	 - ввести сообщение о выходе и вернуться.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: добавьте любой код отрисовки...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
  	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
