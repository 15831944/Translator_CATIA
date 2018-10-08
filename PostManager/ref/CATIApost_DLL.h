// ���� ifdef ����� DLL���� ���������ϴ� �۾��� ���� �� �ִ� ��ũ�θ� ����� 
// ǥ�� ����Դϴ�. �� DLL�� ��� �ִ� ������ ��� ����ٿ� ���ǵ� _EXPORTS ��ȣ��
// �����ϵǸ�, ������ DLL�� ����ϴ� �ٸ� ������Ʈ������ �� ��ȣ�� ������ �� �����ϴ�.
// �̷��� �ϸ� �ҽ� ���Ͽ� �� ������ ��� �ִ� �ٸ� ��� ������Ʈ������ 
// DLLUGPOST_API �Լ��� DLL���� �������� ������ ����, �� DLL��
// �� DLL�� �ش� ��ũ�η� ���ǵ� ��ȣ�� ���������� ������ ���ϴ�.
#ifdef CATIAPOST_DLL_EXPORTS
#define DLLCATIAPOST_API extern "C" __declspec(dllexport)
#else
#define DLLCATIAPOST_API extern "C" __declspec(dllimport)
#endif

#include <atlbase.h>
#include <atlstr.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <comutil.h>
#include <cmath>

#import "C:\\Program Files (x86)\\MACRO\\TransCAD\\TransCAD.exe"

#include "Definition.h"

using namespace std;
namespace Post {
	extern TransCAD::IApplicationPtr g_spApplication;
	class CATIAReferenceManager;
	extern CATIAReferenceManager *g_pRefManager;

	// �� Ŭ������ dllUGPost.dll���� ������ ���Դϴ�.
	DLLCATIAPOST_API int CATIAPostStartService(void);
	DLLCATIAPOST_API int CATIAPostTranslate(char argv[]);
}