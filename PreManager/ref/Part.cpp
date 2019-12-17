#include "stdafx.h"
#include "Part.h"

#include "Feature.h"
#include "FSketch.h"
#include "FeatureSOLIDCreateExtrude.h"
#include "FeatureSOLIDCreateSweep.h"
#include "FeatureSOLIDCreateRevolve.h"
#include "FeatureSOLIDCreateFilletConstant.h"
#include "FeatureSOLIDCreateChamfer.h"
#include "FeatureSOLIDCreatePatternRectangular.h"
#include "FeatureSOLIDCreatePatternCircular.h"
#include "FeatureSOLIDCreateHole.h"
#include "FDatumPlaneOffset.h"

#include "ReferenceEntity.h"

#include "AttributeItem.h"
#include "AttAngle.h"
#include "AttLength.h"
#include "AttLimit.h"
#include "AttParameter.h"
#include "AttAngularRepartation.h"
#include "AttLinearRepartation.h"
#include "AttIntParam.h"

int _planeDatumNumber = 0;

namespace Pre {
	Part::Part(string fileName)
	{
		_spDocuments = g_spApplication->Documents;
		_spDocument = _spDocuments->AddPartDocument();
		_spPart = _spDocument->Part;		// Set TransCAD Part Ptr
		_spFeatures = _spPart->Features;	// Set TransCAD Features Ptr
		_filePath = fileName;

		int start = fileName.find_last_of("\\");
		int end = fileName.find(".CATScript");
		fileName.erase(end);

		_fileName = fileName.substr(start + 1);
		_partDocNum = _spDocuments->GetCount();

		_bstr_t obj(_fileName.c_str());
		_spPart->Name = obj;

		fin.open(_filePath, ios::in);		// �ش� ����� CATIA ��ũ�� ���� �б�
	}
	Part::Part(string fileName, int partNum)
	{
		if (partNum != -1) {
			_filePath = fileName;

			int start = fileName.find_last_of("\\");
			int end = fileName.find(".CATScript");
			fileName.erase(end);

			_fileName = fileName.substr(start + 1);
			_partDocNum = partNum;

			_bstr_t obj(_fileName.c_str());
			//_spPart->set_Name(obj);

			fin.open(_filePath, ios::in);		// �ش� ����� CATIA ��ũ�� ���� �б�

		}
		else {
			return;
		}
		//_spDocuments = g_spApplication->Documents;
		//_spDocument = partdoc;//_spDocuments->AddPartDocument();
		//_spPart = assem_part;		// Set TransCAD Part Ptr
		//_spFeatures = _spPart->Features;	// Set TransCAD Features Ptr
		

	}
	Part::Part(string fileName, TransCAD::IPartDocumentPtr partdoc)
	{
		//_spDocuments = g_spApplication->Documents;
		_spDocument = partdoc;//_spDocuments->AddPartDocument();
		_spPart = _spDocument->Part;		// Set TransCAD Part Ptr
		_spFeatures = _spPart->Features;	// Set TransCAD Features Ptr
		_filePath = fileName;

		int start = fileName.find_last_of("\\");
		int end = fileName.find(".CATScript");
		fileName.erase(end);

		_fileName = fileName.substr(start + 1);
		//_partDocNum = _spDocuments->GetCount();

		_bstr_t obj(_fileName.c_str());
		_spPart->Name = obj;

		fin.open(_filePath, ios::in);		// �ش� ����� CATIA ��ũ�� ���� �б�

	}


	Part::~Part(void)
	{
		fin.close();
		Clear();

		_planeDatumNumber = 0;

		// Clear TransCAD
		g_spApplication = NULL;
		_spDocuments = NULL;
		_spDocument = NULL;
		_spPart = NULL;
		_spFeatures = NULL;
		::CoUninitialize();			// COM Automation API Uninitialization
	}

	void Part::Clear()
	{
		// Reference ����Ʈ ����
		std::vector<ReferenceEntity *>::iterator j = _referenceEntityList.begin();
		while (j != _referenceEntityList.end())
		{
			delete *j;
			j++;
		}
		_referenceEntityList.clear();

		// Feature ����Ʈ ����
		std::vector<Feature *>::iterator i = _featureList.begin();
		while (i != _featureList.end())
		{
			delete *i;
			i++;
		}
		_featureList.clear();

		// Attribute ����Ʈ ����
		std::vector<AttributeItem *>::iterator k = _attributeItemList.begin();
		while (k != _attributeItemList.end())
		{
			delete *k;
			k++;
		}
		_attributeItemList.clear();
	}

	void Part::CreateFeature(string type, string name, ifstream &is)
	{
		Feature * pFeature = 0;

		if (type == "Sketch") //Sketch : 0
			pFeature = new FSketch(this, (int)0, name);
		else if (type == "Pad") //Pad : 1
			pFeature = new FeatureSOLIDCreateExtrude(this, 1, name);
		else if (type == "Pocket") //Pocket : 2
			pFeature = new FeatureSOLIDCreateExtrude(this, 2, name);
		else if (type == "ConstRadEdgeFillet") //EdgeFillet : 3
			pFeature = new FeatureSOLIDCreateFilletConstant(this, 3, name);
		else if (type == "Rib") //Rib : 4
			pFeature = new FeatureSOLIDCreateSweep(this, 4, name);
		else if (type == "Shaft") //Shaft : 5
			pFeature = new FeatureSOLIDCreateRevolve(this, 5, name);
		else if (type == "Groove") //Groove : 6
			pFeature = new FeatureSOLIDCreateRevolve(this, 6, name);
		else if (type == "Chamfer") //Chamfer : 7
			pFeature = new FeatureSOLIDCreateChamfer(this, 7, name);
		else if (type == "HybridShapePlaneOffset") //DatumPlaneOffset : 8
		{
			_planeDatumNumber++;

			char buf[256];
			sprintf_s(buf, sizeof(buf), "Plane.%d", _planeDatumNumber);
			name = buf; // �� ��, CATIApost���� ��ȯ�� ���ؼ� CATIA�� ���̹� ���� ����.

			pFeature = new FDatumPlaneOffset(this, 8, name);
		}
		else if (type == "RectPattern") //Rectangular Pattern : 9
			pFeature = new FeatureSOLIDCreatePatternRectangular(this, 9, name);
		else if (type == "CircPattern") //Circular Pattern : 10
			pFeature = new FeatureSOLIDCreatePatternCircular(this, 10, name);
		else if (type == "Hole") //HoleCounterbored or HoleCountersunk : 11
			pFeature = new FeatureSOLIDCreateHole(this, 11, name);
		else if (type == "Slot")
			pFeature = new FeatureSOLIDCreateSweep(this, 12, name);

		if (pFeature)
		{
			pFeature->GetInfo(is);
			_featureList.push_back(pFeature);
		}
	}

	void Part::CreateAttributeItem(string type, string name, ifstream &is)
	{
		AttributeItem * pAttributeItem = 0;

		if (type == "Length") //Length  
			pAttributeItem = new AttLength(this, 1, name);
		else if (type == "Angle") //Angle  
			pAttributeItem = new AttAngle(this, 2, name);
		else if (type == "Limit")
			pAttributeItem = new AttLimit(this, 3, name);
		else if (type == "Parameter")
			pAttributeItem = new AttParameter(this, 4, name);
		else if (type == "LinearRepartition")
			pAttributeItem = new AttLinearRepartation(this, 5, name);
		else if (type == "AngularRepartition")
			pAttributeItem = new AttAngularRepartation(this, 6, name);
		else if (type == "IntParam")
			pAttributeItem = new AttIntParam(this, 7, name);

		if (pAttributeItem)
		{
			pAttributeItem->GetInfo(is);
			_attributeItemList.push_back(pAttributeItem);
		}
	}

	void Part::CreateReferenceEntity(string type, string name, ifstream &is)
	{
		ReferenceEntity * pReference = 0;

		if (type == "Reference")
			pReference = new ReferenceEntity(this, (int)0, name);
		else if (type == "AnyObject" || type == "HybridShape" || type == "CATBaseDispatch")
			pReference = new ReferenceEntity(this, (int)1, name);

		if (pReference)
		{
			pReference->GetInfo(is);
			_referenceEntityList.push_back(pReference);
		}
	}

	void Part::GetInfo()
	{
		char buffer[1000];				// CATIA ��ũ�� ������ ������ �ӽ÷� �����ϴ� �迭
		char seps[] = " .=";			// ������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����
		string token, name, type;		// ���ڿ� token, ��ü �̸� �� Ÿ��

		try
		{
			while (!fin.eof()) // CATIA ��ũ�� ������ ������ ���� �б� ����
			{
				fin.getline(buffer, 1000);   //�� ������ �д´�.
				strcpy_s(buffer,stringLibrary::ReplaceAll(buffer, "sketch", "Sketch").c_str());//sketch�� Sketch�� ��ȯ (TransCAD ���ο��� Sketch�� �����͸� ó���ϴµ�)


				if (strlen(buffer) == 0)  // �� ������ ���� ���
				{
				}
				else if (!strncmp(buffer, "Dim", 3))  //ù��° �ܾ Dim�� ���.
				{
					token = strtok_s(buffer, seps, &context);	//Dim
					token = strtok_s(NULL, seps, &context);	//pad1
					name = token; //��ü�� �̸� ����

					token = strtok_s(NULL, seps, &context);	//As
					token = strtok_s(NULL, seps, &context);	//Pad
					type = token; //��ü�� Ÿ�� ����

					CreateReferenceEntity(type, name, fin);	// ���۷��� ����
					CreateFeature(type, name, fin);			// Ư¡���� ����	
					CreateAttributeItem(type, name, fin);	// �Ӽ� ����
				}
				else if (!strncmp(buffer, "Set", 3))
				{
					token = strtok_s(buffer, seps, &context);	//Set
					token = strtok_s(NULL, seps, &context);	//factory2D11
					token = strtok_s(NULL, seps, &context);	//sketch1
					name = token; //��ü�� �̸� ����

					token = strtok_s(NULL, seps, &context);	//OpenEdition()

					// ����ġ�� �ٽ� ���� �۾��� �� ����
					if (token == "OpenEdition()")
					{
						FSketch * pFsketch = (FSketch *)GetFeatureByName(name);
						pFsketch->GetSketchItemInfo(fin);
					}
				}
				else
				{
					//�Ʒ��� ������ ���� Script ���Ͽ� �ٷ� Class �ν��Ͻ� �̸��� ���ö� ���
					//FeatureManager�� AttributeManager�� �̸��� ���� ���� ������
					//�� Feature�� Attribute Class�� Modify�Լ� ȣ���Ͽ� ������Ʈ.
					//	length1.Value = 40.000000
					//	chamfer1.Propagation = 0
					//	angle2.Value = 10.000000
					//	pad2.DirectionOrientation = 1
					char temp[1000];
					strcpy_s(temp, buffer);

					token = strtok_s(buffer, seps, &context);
					name = token;

					if (GetFeatureByName(name))						//Feature�� �ִ��� Ȯ��
						GetFeatureByName(name)->Modify(temp);		//���� ����
					else if (GetAttributeItemByName(name))			//Attribute�� �ִ��� Ȯ��
						GetAttributeItemByName(name)->Modify(temp);	//���� ����
				}
			}

			context = NULL;
		}
		catch (...)
		{
		}
	}

	void Part::ToTransCAD()
	{
		// Ư¡���� TransCAD �� ��ȯ
		for (int i = 0; (unsigned int)i < GetSize(); ++i)
		{
			GetFeature(i)->ToTransCAD();
		}
	}

	// Feature�� �̸��� �Է¹޾� �ش� Feature �����͸� ����
	Feature * Part::GetFeatureByName(string name)
	{
		for (size_t i = 0; (unsigned int)i < GetSize(); ++i)
		{
			Feature * pFeature = GetFeature((int)i);

			if (name == pFeature->GetFeatureName())
				return pFeature;
		}

		return NULL;
	}

	// ReferenceEntity�� �̸��� �Է¹޾� �ش� ReferenceEntity �����͸� ����
	ReferenceEntity * Part::GetReferenceEntityByName(string name)
	{
		for (size_t i = 0; (unsigned int)i < _referenceEntityList.size(); ++i)
		{
			ReferenceEntity * _pReferenceEntity = GetReferenceEntity((int)i);

			if (name == _pReferenceEntity->GetName())
				return _pReferenceEntity;
		}

		return NULL;
	}

	// AttributeItem�� �̸��� �Է¹޾� �ش� AttributeItem �����͸� ����
	AttributeItem * Part::GetAttributeItemByName(string name)
	{
		for (size_t i = 0; (unsigned int)i < _attributeItemList.size(); ++i)
		{
			AttributeItem * _pAttributeItem = GetAttributeItem((int)i);

			if (name == _pAttributeItem->GetName())
				return _pAttributeItem;
		}

		return NULL;
	}

	// Sketch-based Feature�� �̸���, �����ϴ� Sketch �̸��� �����ϱ� ���� �뵵�� ����
	void Part::AddFeatureNameSketchName(string featureName, string sketchName)
	{
		SketchFeature _sketchFeature;
		_sketchFeature._featureName = featureName;
		_sketchFeature._sketchName = sketchName;

		sketchNameTable.push_back(_sketchFeature);
	}

	// Sketch-based Feature�� �̸��� �Է��Ͽ� �����ϴ� Sketch �̸��� ����ϱ� ���� �뵵�� ����
	string Part::GetSketchNameFromFeatureName(string featureName)
	{
		string sketchName;
		for (int i = 0; (unsigned int)i < sketchNameTable.size(); ++i)
		{
			if (sketchNameTable[i]._featureName == featureName)
				sketchName = sketchNameTable[i]._sketchName;
		}

		return sketchName;
	}

	// Sketch�� �̸��� �Է��Ͽ� �����ϴ� Sketch-based Feature�̸��� ����ϱ� ���� �뵵�� ����
	string Part::GetFeatureNameFromSketchName(string sketchName)
	{
		string featureName;

		for (int i = 0; (unsigned int)i < sketchNameTable.size(); ++i)
		{
			if (sketchNameTable[i]._sketchName == sketchName)
				featureName = sketchNameTable[i]._featureName;
		}

		return featureName;
	}
}