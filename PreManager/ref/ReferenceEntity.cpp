#include "stdafx.h"
#include "ReferenceEntity.h"

namespace Pre {
	ReferenceEntity::ReferenceEntity(Part * pPart, int _type, string _name)
	{
		_pPart = pPart;
		name = _name;
		type = _type;

		pSketchManager = 0; // ����ġ �Ŵ��� �ʱ�ȭ. ����ġ ������Ʈ �˻� �� Ȱ��
	}

	ReferenceEntity::~ReferenceEntity()
	{
	}

	void ReferenceEntity::GetInfo(ifstream & is)
	{
		// ReferenceEntity�� ������ �����ϴ� �Լ�
		char buffer[1000];

		f_Getline_Custom(is, buffer, 500)
		while (strncmp(buffer, "Set", 3))
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string referenceValue;
		char seps_temp[] = " :;,\t\n()=."; //������
		char * context = NULL;

		referenceValue = strtok_s(buffer, seps_temp, &context);	//ù��° �ܾ�
		referenceValue = strtok_s(NULL, seps_temp, &context);	//�ι�° 
		referenceValue = strtok_s(NULL, seps_temp, &context);	//����° 

		// Default DatumPlane�� ���
		if (referenceValue == "originElements1")
		{
			referenceValue = strtok_s(NULL, seps_temp, &context);	//�׹�°

			if (referenceValue == "PlaneXY" || referenceValue == "PlaneYZ" || referenceValue == "PlaneZX")
			{
				type = 1;
				feature = referenceValue;
			}
		}
		// DatumPlaneOffset �� ���
		else if (referenceValue.find("hybridShapes") == 0)
		{
			referenceValue = strtok_s(NULL, seps_temp, &context);	//�׹�° 

			if (referenceValue == "Item")
			{
				char seps_temp2[] = " \",\t\n()=."; //������
				referenceValue = strtok_s(NULL, seps_temp2, &context); //Plane

				if (!strncmp(referenceValue.c_str(), "Plane", 5))
				{
					referenceValue = strtok_s(NULL, seps_temp2, &context);  //1
					referenceValue = "Plane." + referenceValue;

					type = 4;
					feature = referenceValue;
				}
			}
		}
		// ����ġ�� Local axis�� ���
		else if (referenceValue.find("axis2D") == 0)
		{
			pWire._sketchName = referenceValue;

			referenceValue = strtok_s(NULL, seps_temp, &context);	//�׹�°

			// VDirection �Ǵ� HDirection�� ���
			if (referenceValue == "GetItem")
			{
				char seps_temp2[] = " \",\t\n()=."; //������
				referenceValue = strtok_s(NULL, seps_temp2, &context); //Plane
				pWire._sketchItemName = referenceValue;
			}
		}
		// Ư¡������ B-rep ��Ҹ� �����ϰų�, Datum Plane ��ü�� �����ϴ� ���
		else if (referenceValue == "part1")
		{
			referenceValue = strtok_s(NULL, seps_temp, &context);	//�׹�°

			if (referenceValue == "CreateReferenceFromName")
			{
				type = 3;  //Surface
				referenceValue = strtok_s(NULL, seps_temp, &context);

				if (referenceValue == "\"Selection_RSur")
				{
					TransCADFaceName = GetFullFaceName(context);
				}
				else  // �ƹ��͵� �������� ���� ���
				{
					type = 0;
				}
			}
			else if (referenceValue == "CreateReferenceFromBRepName")
			{
				referenceValue = strtok_s(NULL, seps_temp, &context);

				if (referenceValue == "\"FSur" || referenceValue == "\"RSur")
				{
					type = 3; //Surface
					TransCADFaceName = GetFullFaceName(context);
				}
				else if (referenceValue == "\"REdge" || referenceValue == "\"TgtEdge" || referenceValue == "\"TgtIntersEdge")
				{
					type = 2; //Edge
					while (referenceValue != "Edge")
					{
						referenceValue = strtok_s(NULL, seps_temp, &context); //Edge
					}

					TransCADEdgeName = GetFullFaceName(context); // ù ��° ���� Face �̸� �߰�

					referenceValue = strtok_s(NULL, seps_temp, &context); //Face

					if (TransCADEdgeName.find(":0;0") != -1)
						referenceValue = strtok_s(NULL, seps_temp, &context); //Brep

					TransCADEdgeName = TransCADEdgeName + "#" + GetFullFaceName(context); // �� ��° ���� Face �̸� �߰�
				}
				else if (referenceValue == "\"WireREdge" || referenceValue == "\"Axis")
				{
					type = 2; //WireEdge

					referenceValue = strtok_s(NULL, seps_temp, &context);
					referenceValue = strtok_s(NULL, seps_temp, &context);
					referenceValue = strtok_s(NULL, seps_temp, &context);

					if (referenceValue == "Sketch")
					{
						referenceValue = strtok_s(NULL, seps_temp, &context);

						pWire._sketchName = "Sketch" + referenceValue; // ����ġ �̸�

						referenceValue = strtok_s(NULL, seps_temp, &context);
						int edgeNumber = atoi(referenceValue.c_str());

						pSketchManager = (FSketch*)_pPart->GetFeatureByName(pWire._sketchName);

						pWire._sketchItemName = pSketchManager->GetEntityNameFromReportName(edgeNumber); // ����ġ ��� �̸�
					}
					else
					{
						referenceValue = strtok_s(NULL, seps_temp, &context);
						referenceValue = strtok_s(NULL, seps_temp, &context);
						referenceValue = strtok_s(NULL, seps_temp, &context);
						referenceValue = strtok_s(NULL, seps_temp, &context);
						referenceValue = strtok_s(NULL, seps_temp, &context);
						referenceValue = strtok_s(NULL, seps_temp, &context);

						pWire._sketchName = "Sketch" + referenceValue; // ����ġ �̸�

						referenceValue = strtok_s(NULL, seps_temp, &context);
						int edgeNumber = atoi(referenceValue.c_str());

						pSketchManager = (FSketch*)_pPart->GetFeatureByName(pWire._sketchName);

						pWire._sketchItemName = pSketchManager->GetEntityNameFromReportName(edgeNumber); // ����ġ ��� �̸�
					}

					pSketchManager = 0;
				}
				else if (referenceValue == "\"WireFVertex")
				{
					type = 5; // Point
					// Loft�� start point ������ �����ϴµ� ���
					// �� �� �߰�
				}
				else
				{
				}
			}
			else if (referenceValue == "CreateReferenceFromObject")
			{
				//�⺻�� type�� 4��� �����ϰ� DatumPoint ó�� ��ƾ�� �߰�
				//Loft���� �����ϴ� Sketch ���� ��Ÿ���� ���
				//�����ϴ� ��ü�� Factory2D �� sketch element�� ���

				referenceValue = strtok_s(NULL, seps_temp, &context); //factory2D2 or sketch5

				type = 4;  //object
				feature = referenceValue;
			}
		}

		context = NULL;
	}

	void ReferenceEntity::ToTransCAD()
	{
		spReferencePtr = NULL;

		// 1=Default datum plane, 2=Edge, 3=Surface, 4=Object, 5=Point  
		if (type == 1)
		{
			if (feature == "PlaneXY")
				spReferencePtr = _pPart->_spPart->SelectObjectByName("XYPlane");
			else if (feature == "PlaneYZ")
				spReferencePtr = _pPart->_spPart->SelectObjectByName("YZPlane");
			else if (feature == "PlaneZX")
				spReferencePtr = _pPart->_spPart->SelectObjectByName("ZXPlane");
		}
		else if (type == 2)
		{
			spReferencePtr = _pPart->_spPart->SelectBrepByName(TransCADEdgeName.c_str());
		}
		else if (type == 3)
		{
			spReferencePtr = _pPart->_spPart->SelectBrepByName(TransCADFaceName.c_str());
		}
		else if (type == 4)
		{
			// DatumPlaneOffset�� Reference�� ��� ���,
			// ex) hybridShapePlaneOffset1 �� Plane.1
			string temp = feature;

			if (!strncmp(feature.c_str(), "hybridShapePlaneOffset", 22))
			{
				int index = 0;
				char refNum[1024], refName[1024];

				for (int i = 0; i < (int)feature.size(); ++i)
				{
					if (feature[i] >= '0' && feature[i] <= '9' && feature[i] != '.')
						refNum[index++] = feature[i];
				}

				int refNumber = atoi(refNum);
				sprintf_s(refName, "Plane.%d", refNumber);
				temp = refName;

				spReferencePtr = _pPart->_spPart->SelectObjectByName(temp.c_str());
			}
			else // Plane.1
				spReferencePtr = _pPart->_spPart->SelectObjectByName(temp.c_str());
		}
		else if (type == 5)
		{
		}
	}

	// CATIA�� FullFaceName�� ���� TransCAD�� FullFaceName�� �����ϴ� �Լ�
	string ReferenceEntity::GetFullFaceName(char *& buffer)
	{
		string referenceValue;
		string _fullFaceName;
		char seps_temp[] = " :;,\t\n()=."; //������

		referenceValue = strtok_s(NULL, seps_temp, &buffer);
		referenceValue = strtok_s(NULL, seps_temp, &buffer);
		string temp_buff = buffer;

		referenceValue = strtok_s(NULL, seps_temp, &buffer);

		// ���� Merge �Ǿ� �ִ� ��� (Secondary Name�� ����)
		if (referenceValue == "Brp")
		{
			vector<string> _faceNameList;

			_faceNameList.push_back(GetBasicFaceName(buffer));

			referenceValue = strtok_s(NULL, seps_temp, &buffer);

			while (referenceValue == "Brp")
			{
				_faceNameList.push_back(GetBasicFaceName(buffer));

				referenceValue = strtok_s(NULL, seps_temp, &buffer);
			}

			// The number of merged faces
			stringstream mergedFaceNum;
			mergedFaceNum << _faceNameList.size() - 1;

			for (int i = _faceNameList.size() - 1; i >= 0; --i)
			{
				if (i == _faceNameList.size() - 1)
					_fullFaceName = _faceNameList[i] + ":0,0:" + mergedFaceNum.str();
				else
					_fullFaceName = _fullFaceName + ";" + _faceNameList[i];
			}
		}
		else // Merge �� ���� ���� ���
		{
			buffer = new char[temp_buff.length() + 1];

			strcpy_s(buffer, temp_buff.length() + 1, temp_buff.c_str());
			string _basicFaceName = GetBasicFaceName(buffer);

			_fullFaceName = _basicFaceName
				+ ":0,0"	// Split Face Information
				+ ":0;0";	// Merge Face Information
		}

		if (type == 2) //Edge ���� ����
		{
			pEdge.push_back(pFace);
			pFace.clear();
		}

		return _fullFaceName;
	}

	// CATIA�� BasicFaceName�� ���� TransCAD�� BasicFaceName�� �����ϴ� �Լ�
	string ReferenceEntity::GetBasicFaceName(char *& buffer)
	{
		string referenceValue;
		char seps_temp[] = " :;,\t\n()=."; //������

		FaceInfo _pFace; //�� ������ �ӽ÷� ����

		referenceValue = strtok_s(NULL, seps_temp, &buffer);

		// Shell�� ��� TransCAD���� ���� �ȵ�.
		// ���̹� ��Ű���� ���� ����. (16-01-18)
		if (referenceValue == "Shell")
		{
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
		}

		if (referenceValue == "Pad")
		{
			_pFace._featureName = "pad";
			_pFace._featureType = "ExtrudeFeature";
		}
		else if (referenceValue == "Pocket")
		{
			_pFace._featureName = "pocket";
			_pFace._featureType = "ExtrudeFeature";
		}
		else if (referenceValue == "Shaft")
		{
			_pFace._featureName = "shaft";
			_pFace._featureType = "RevolveFeature";
		}
		else if (referenceValue == "Groove")
		{
			_pFace._featureName = "groove";
			_pFace._featureType = "RevolveFeature";
		}
		else if (referenceValue == "Rib")
		{
			_pFace._featureName = "rib";
			_pFace._featureType = "SweepFeature";
		}
		else if (referenceValue == "Slot")
		{
			_pFace._featureName = "slot";
			_pFace._featureType = "SweepFeature";
		}

		referenceValue = strtok_s(NULL, seps_temp, &buffer); // Feature ��ȣ
		_pFace._featureName = _pFace._featureName + referenceValue;

		referenceValue = strtok_s(NULL, seps_temp, &buffer);
		int _faceType = atoi(referenceValue.c_str());

		_pFace._sketchName2 = "0";
		_pFace._sketchItemName2 = "0";

		if (_faceType == 0) // 0:����, 1:�Ʒ���, 2:����
		{
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			_pFace._sketchName1 = "Sketch" + referenceValue;

			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			int _sketchItemNum1 = atoi(referenceValue.c_str());

			pSketchManager = (FSketch*)_pPart->GetFeatureByName(_pFace._sketchName1);
			_pFace._sketchItemName1 = pSketchManager->GetEntityNameFromReportName(_sketchItemNum1);

			// rib�� �ƴϰ� sketch ������ 2�� �ʿ��� ��쿡�� ó�� �ʿ�
			// ����� rib�� ��츸 ó��
			if (_pFace._featureType == "SweepFeature")
			{
				referenceValue = strtok_s(NULL, seps_temp, &buffer);

				if (referenceValue != "None")
				{
					referenceValue = strtok_s(NULL, seps_temp, &buffer);
					referenceValue = strtok_s(NULL, seps_temp, &buffer);
					_pFace._sketchName2 = "Sketch" + referenceValue;

					referenceValue = strtok_s(NULL, seps_temp, &buffer);
					int _sketchItemNum2 = atoi(referenceValue.c_str());

					pSketchManager = (FSketch*)_pPart->GetFeatureByName(_pFace._sketchName2);
					_pFace._sketchItemName2 = pSketchManager->GetEntityNameFromReportName(_sketchItemNum2);
				}
			}
		}
		else if (_faceType == 1)
		{
			_faceType = -1;
			_pFace._sketchName1 = "0";
			_pFace._sketchItemName1 = "-1";
		}
		else if (_faceType == 2)
		{
			_faceType = -2;
			_pFace._sketchName1 = "0";
			_pFace._sketchItemName1 = "-2";
		}

		// ���� basic face name
		string _basicFaceName = _pFace._featureName + ","
			+ _pFace._sketchName1 + ","
			+ _pFace._sketchItemName1 + ","
			+ _pFace._sketchName2 + ","
			+ _pFace._sketchItemName2 + ","
			+ "0,"
			+ _pFace._featureType;


		pFace.push_back(_pFace);

		return _basicFaceName;
	}

	//Assembly��
	/*
		���� �� ������ ���� �ƴϰ� ReferenceFromName, ReferenceFromBrepName �� ���� �͸� ������
	*/
	string ReferenceEntity::GetReferName(char *& context) {
	
		
		string referenceValue;
		char seps_temp[] = " :;,\t\n()=."; //������
		//char * context = NULL;

		string s;

#pragma region GetInfo  �κ� ����

		referenceValue = strtok_s(NULL, seps_temp, &context);

		//CreateReferenceFromName*******************************/
		if (referenceValue == "\"Selection_RSur")
		{
			type = 3;  //Surface
			TransCADFaceName = GetFullFaceName(context);
		}
		
		//CreateReferenceFromBRepName*******************************/
		else if (referenceValue == "\"FSur" || referenceValue == "\"RSur")
		{
			type = 3; //Surface
			TransCADFaceName = GetFullFaceName(context);
		}
		else if (referenceValue == "\"REdge" || referenceValue == "\"TgtEdge" || referenceValue == "\"TgtIntersEdge")
		{
			type = 2; //Edge
			while (referenceValue != "Edge")
			{
				referenceValue = strtok_s(NULL, seps_temp, &context); //Edge
			}

			TransCADEdgeName = GetFullFaceName(context); // ù ��° ���� Face �̸� �߰�

			referenceValue = strtok_s(NULL, seps_temp, &context); //Face

			if (TransCADEdgeName.find(":0;0") != -1)
				referenceValue = strtok_s(NULL, seps_temp, &context); //Brep

			TransCADEdgeName = TransCADEdgeName + "#" + GetFullFaceName(context); // �� ��° ���� Face �̸� �߰�
		}
		else if (referenceValue == "\"WireREdge" || referenceValue == "\"Axis")
		{
			type = 2; //WireEdge

			referenceValue = strtok_s(NULL, seps_temp, &context);
			referenceValue = strtok_s(NULL, seps_temp, &context);
			referenceValue = strtok_s(NULL, seps_temp, &context);

			if (referenceValue == "Sketch")
			{
				referenceValue = strtok_s(NULL, seps_temp, &context);

				pWire._sketchName = "Sketch" + referenceValue; // ����ġ �̸�

				referenceValue = strtok_s(NULL, seps_temp, &context);
				int edgeNumber = atoi(referenceValue.c_str());

				pSketchManager = (FSketch*)_pPart->GetFeatureByName(pWire._sketchName);

				pWire._sketchItemName = pSketchManager->GetEntityNameFromReportName(edgeNumber); // ����ġ ��� �̸�
			}
			else
			{
				referenceValue = strtok_s(NULL, seps_temp, &context);
				referenceValue = strtok_s(NULL, seps_temp, &context);
				referenceValue = strtok_s(NULL, seps_temp, &context);
				referenceValue = strtok_s(NULL, seps_temp, &context);
				referenceValue = strtok_s(NULL, seps_temp, &context);
				referenceValue = strtok_s(NULL, seps_temp, &context);

				pWire._sketchName = "Sketch" + referenceValue; // ����ġ �̸�

				referenceValue = strtok_s(NULL, seps_temp, &context);
				int edgeNumber = atoi(referenceValue.c_str());

				pSketchManager = (FSketch*)_pPart->GetFeatureByName(pWire._sketchName);

				pWire._sketchItemName = pSketchManager->GetEntityNameFromReportName(edgeNumber); // ����ġ ��� �̸�
			}

			pSketchManager = 0;
		}
		else if (referenceValue == "\"WireFVertex")
		{
			type = 5; // Point
					  // Loft�� start point ������ �����ϴµ� ���
					  // �� �� �߰�
		}
		else
		{	// �ƹ��͵� �������� ���� ���
			type = 0;

		}

#pragma endregion

#pragma region ToTransCAD  �κ� ����
		// 1=Default datum plane, 2=Edge, 3=Surface, 4=Object, 5=Point  
		if (type == 1)
		{
			if (feature == "PlaneXY")
				s = "XYPlane";
			else if (feature == "PlaneYZ")
				s = "YZPlane";
			else if (feature == "PlaneZX")
				s = "ZXPlane";
		}
		else if (type == 2)
		{
			s = TransCADEdgeName.c_str();
		}
		else if (type == 3)
		{
			s = TransCADFaceName.c_str();
		}
		else if (type == 4)
		{
			// DatumPlaneOffset�� Reference�� ��� ���,
			// ex) hybridShapePlaneOffset1 �� Plane.1
			string temp = feature;

			if (!strncmp(feature.c_str(), "hybridShapePlaneOffset", 22))
			{
				int index = 0;
				char refNum[1024], refName[1024];

				for (int i = 0; i < (int)feature.size(); ++i)
				{
					if (feature[i] >= '0' && feature[i] <= '9' && feature[i] != '.')
						refNum[index++] = feature[i];
				}

				int refNumber = atoi(refNum);
				sprintf_s(refName, "Plane.%d", refNumber);
				temp = refName;

				s = temp.c_str();
			}
			else // Plane.1
				s = temp.c_str();
		}
		else if (type == 5)
		{
		}

#pragma endregion

		return s;
	}
}