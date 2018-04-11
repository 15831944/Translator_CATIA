// ���� ifdef ����� DLL���� ���������ϴ� �۾��� ���� �� �ִ� ��ũ�θ� ����� 
// ǥ�� ����Դϴ�. �� DLL�� ��� �ִ� ������ ��� ����ٿ� ���ǵ� _EXPORTS ��ȣ��
// �����ϵǸ�, ������ DLL�� ����ϴ� �ٸ� ������Ʈ������ �� ��ȣ�� ������ �� �����ϴ�.
// �̷��� �ϸ� �ҽ� ���Ͽ� �� ������ ��� �ִ� �ٸ� ��� ������Ʈ������ 
// DLLUGPOST_API �Լ��� DLL���� �������� ������ ����, �� DLL��
// �� DLL�� �ش� ��ũ�η� ���ǵ� ��ȣ�� ���������� ������ ���ϴ�.
#ifdef CATIAPRE_DLL_EXPORTS
#define DLLCATIAPRE_API extern "C" __declspec(dllexport)
#else
#define DLLCATIAPRE_API extern "C" __declspec(dllimport)
#endif

#import "C:\\Program Files (x86)\\MACRO\\TransCAD\\TransCAD.exe"

#include <stdio.h>
#include <tchar.h>

#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <math.h>
#include <sstream>
#include <comutil.h>
#include <comdef.h>

using namespace std;
#include "Definition.h"
namespace Pre {
	extern TransCAD::IApplicationPtr g_spApplication;
	// �� Ŭ������ dllUGPost.dll���� ������ ���Դϴ�.
	DLLCATIAPRE_API int CATIAPreStartService(void);
	DLLCATIAPRE_API int CATIAPreTranslate(char * path);
	DLLCATIAPRE_API int CATIAPreTranslateAssem(int totPartNum, int partDocNum[], double partCoords[][12]);
}