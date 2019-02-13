#pragma once
#include <stdio.h>
#include <strstream>
#include <iostream>

using std::string;
using std::ostrstream;



#define f_Getline_Custom(s,b,n) strcpy_s(b,stringLibrary::_Getline_Custom(s,n).c_str());


namespace Pre {

	class stringLibrary {

	public:
		static string LtoU(string source, int index) {

			string src = source;
			char f = src.at(index);

			if (2 == islower(f)) { // �ҹ����̸�

				src[index] = f - ('a' - 'A');

			}

			return src;

		}

		static string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
			size_t start_pos = 0; //stringó������ �˻�
			while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from�� ã�� �� ���� ������
			{
				str.replace(start_pos, from.length(), to);
				start_pos += to.length(); // �ߺ��˻縦 ���ϰ� from.length() > to.length()�� ��츦 ���ؼ�
			}
			return str;
		}

		static string _Getline_Custom(std::ifstream &_is, int n) {

			char* s_buff = new char[n];

			_is.getline(s_buff, 500);

			return ReplaceAll(s_buff, "sketch", "Sketch");//sketch�� Sketch�� ��ȯ (TransCAD ���ο��� Sketch�� �����͸� ó���ϴµ�)

		}


	};
}