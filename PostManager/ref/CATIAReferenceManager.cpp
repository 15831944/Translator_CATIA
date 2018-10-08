#include "stdafx.h"
#include "CATIAReferenceManager.h"

namespace Post {
	CATIAReferenceManager::CATIAReferenceManager(void)
	{
		_id = 0;
	}

	CATIAReferenceManager::~CATIAReferenceManager(void)
	{



	}

	// ��� Sketch Entity(Point, Line ��)�� ����
	int CATIAReferenceManager::AddNewEntity(std::string strSketchName, std::string strTransCADName)
	{
		//���� ���ο� ����ġ�� �߰��Ǵ� ��� _id �� �ʱ�ȭ
		int count = 0;
		for (int i = 0; i < (int)_listEntityNameMap.size(); ++i)
		{
			if (_listEntityNameMap[i]._sketchName == strSketchName)
				count++;
		}

		if (count == 0)
			_id = 0;

		_id++;
		ENTITY_MAP map;
		map._sketchName = strSketchName;
		map.id = _id;
		map._entityName = strTransCADName;
		_listEntityNameMap.push_back(map);

		_listEntityID.push_back(_id);
		_listTransCADName.push_back(strTransCADName);

		return _id;
	}

	// ��� Sketch, Feature, DatumPlane ������ ����
	int CATIAReferenceManager::AddNewFeature(TransCAD::IFeaturePtr spFeature)
	{
		std::string name = (std::string)spFeature->Name;
		TransCAD::FeatureType type = spFeature->Type;

		int id = 0;
		for (int i = 0; i < (int)_listNameMap.size(); ++i)
		{
			if (_listNameMap[i]._type == type)
				id++;
		}

		id++;

		NAME_MAP map;
		map._id = id;
		map._type = type;
		map._name = name;

		_listNameMap.push_back(map);

		return 1;
	}

	// EntityID�� TransCADName �˻�
	int CATIAReferenceManager::GetTransCADNameByEntityID(std::string strSketchName, int id, std::string &strTransCADName)
	{
		for (int i = 0; i < (int)_listEntityNameMap.size(); ++i)
		{
			if (_listEntityNameMap[i]._sketchName == strSketchName && _listEntityNameMap[i].id == id)
			{
				strTransCADName = _listEntityNameMap[i]._entityName;
				return 1;
			}
		}

		return 0;
	}

	// TransCADName���� EntityID �˻�
	int CATIAReferenceManager::GetEntityIDByTransCADName(std::string strSketchName, std::string strTransCADName, int &id)
	{
		for (int i = 0; i < (int)_listEntityNameMap.size(); ++i)
		{
			if (_listEntityNameMap[i]._entityName == strTransCADName
				&& _listEntityNameMap[i]._sketchName == strSketchName
				&& i > 1)
			{
				id = _listEntityNameMap[i].id;
				return 1;
			}
		}

		return 0;
	}

	// TransCADName���� FeatureID �˻�
	int CATIAReferenceManager::GetFeatureIDByTransCADName(std::string strTransCADName, int &id)
	{
		for (int i = 0; i < (int)_listNameMap.size(); ++i)
		{
			if (_listNameMap[i]._name == strTransCADName)
			{
				id = _listNameMap[i]._id;
				return 1;
			}
		}

		return 0;
	}

	// TransCADName���� FeatureType �˻�
	int CATIAReferenceManager::GetFeatureTypeByTransCADName(std::string strTransCADName, TransCAD::FeatureType &type)
	{
		for (int i = 0; i < (int)_listNameMap.size(); ++i)
		{
			if (_listNameMap[i]._name == strTransCADName)
			{
				type = _listNameMap[i]._type;
				return 1;
			}
		}

		return 0;
	}

	// ��� Point�� �̸� ����. CATIA������ �̸�, TransCAD������ �̸��� ���ÿ� ����
	void CATIAReferenceManager::AddNewPointName(std::string strSketchName, std::string CATIApointName, std::string TransCADpointName)
	{
		POINTNAME_MAP map;
		map._sketchName = strSketchName;
		map._CATIAName = CATIApointName;
		map._TransCADName = TransCADpointName;

		_listPointNameMap.push_back(map);
	}

	// TransCAD������ point �̸����� CATIA������ point �̸� �˻�
	std::string CATIAReferenceManager::GetCatiaPointNameByTransCADPointName(std::string strSketchName, std::string TransCADpointName)
	{
		string CATIAPointName;

		for (int i = 0; i < (int)_listPointNameMap.size(); ++i)
		{
			if (_listPointNameMap[i]._sketchName == strSketchName && _listPointNameMap[i]._TransCADName == TransCADpointName)
			{
				CATIAPointName = _listPointNameMap[i]._CATIAName;
			}
		}

		return CATIAPointName;
	}

	// TransCAD�� FeatureType�� �Է¹޾� CATIA�� FeatureType�� string���� ����
	std::string CATIAReferenceManager::FeatureTypeToString(TransCAD::FeatureType type)
	{
		switch (type)
		{
		case TransCAD::StdSolidProtrusionExtrudeFeature:
			return std::string("Pad");
		case TransCAD::StdSolidCutExtrudeFeature:
			return std::string("Pocket");
		case TransCAD::StdSketchFeature:
			return std::string("Sketch");
		case TransCAD::StdSolidProtrusionSweepFeature:
			return std::string("Rib");
		case TransCAD::StdSolidProtrusionRevolveFeature:
			return std::string("Shaft");
		case TransCAD::StdSolidCutRevolveFeature:
			return std::string("Groove");
		default:
			return std::string("None");
		}
	}

	// TransCAD Face�� ���� ��Ī�� �޾Ƽ� CATIA Face�� ���� ��Ī�� ���
	std::string CATIAReferenceManager::ConvertToBRepFace(std::string strPersistentName)
	{
		EN_F f;
		Parsing_F(strPersistentName, f);

		TransCAD::FeatureType type;
		int id;
		char buf[1024];

		GetFeatureTypeByTransCADName(f._bn.feature_id, type);
		GetFeatureIDByTransCADName(f._bn.feature_id, id);

		if (f._sn.total_num != 0)
		{
			GetFeatureTypeByTransCADName(f._sn._bn_list[f._sn.total_num - 1].feature_id, type);
			GetFeatureIDByTransCADName(f._sn._bn_list[f._sn.total_num - 1].feature_id, id);
		}

		sprintf_s(buf, "%s.%d", FeatureTypeToString(type).c_str(), id);
		std::string feature_name(buf);

		std::string faceName, finalName;
		faceName = GetFaceNameOfCATIA(f);

		finalName = "Selection_RSur:(Face:(Brp:("
			+ faceName
			+ ");None:();Cf11:());"
			+ feature_name
			+ "_ResultOUT;Z0;G3563)";

		return finalName;
	}

	// TransCAD Face�� ���� ��Ī�� �޾Ƽ� CATIA Face�� ���� ��Ī�� ��� (HoleCounterbored�� HoleCountersunk ����)
	std::string CATIAReferenceManager::ConvertToBRepFaceForHole(std::string strPersistentName)
	{
		EN_F f;
		Parsing_F(strPersistentName, f);

		TransCAD::FeatureType type;
		int id;
		char buf[1024];

		GetFeatureTypeByTransCADName(f._bn.feature_id, type);
		GetFeatureIDByTransCADName(f._bn.feature_id, id);

		if (f._sn.total_num != 0)
		{
			GetFeatureTypeByTransCADName(f._sn._bn_list[f._sn.total_num - 1].feature_id, type);
			GetFeatureIDByTransCADName(f._sn._bn_list[f._sn.total_num - 1].feature_id, id);
		}

		if (FeatureTypeToString(type) == "Pad")
			sprintf_s(buf, "%s%d", "pad", id);
		else if (FeatureTypeToString(type) == "Rib")
			sprintf_s(buf, "%s%d", "rib", id);
		else if (FeatureTypeToString(type) == "Pocket")
			sprintf_s(buf, "%s%d", "pocket", id);
		else if (FeatureTypeToString(type) == "Shaft")
			sprintf_s(buf, "%s%d", "shaft", id);
		else if (FeatureTypeToString(type) == "Groove")
			sprintf_s(buf, "%s%d", "groove", id);

		std::string feature_name(buf);

		std::string faceName, finalName;
		faceName = GetFaceNameOfCATIA(f);

		finalName = "FSur:(Face:(Brp:("
			+ faceName
			+ ");None:();Cf11:());WithTemporaryBody;WithoutBuildError;WithInitialFeatureSupport;MonoFond;MFBRepVersion_CXR15)\", "
			+ feature_name;

		return finalName;
	}

	// TransCAD Face�� ���� ��Ī�� �޾Ƽ� CATIA Face�� ���� ��Ī�� ��� (DatumPlaneOffset ����) 
	std::string CATIAReferenceManager::ConvertToBRepFaceForDatumPlane(std::string strPersistentName)
	{
		EN_F f;
		Parsing_F(strPersistentName, f);

		TransCAD::FeatureType type;
		int id;
		char buf[1024];

		GetFeatureTypeByTransCADName(f._bn.feature_id, type);
		GetFeatureIDByTransCADName(f._bn.feature_id, id);

		if (f._sn.total_num != 0)
		{
			GetFeatureTypeByTransCADName(f._sn._bn_list[f._sn.total_num - 1].feature_id, type);
			GetFeatureIDByTransCADName(f._sn._bn_list[f._sn.total_num - 1].feature_id, id);
		}

		if (FeatureTypeToString(type) == "Pad")
			sprintf_s(buf, "%s%d", "pad", id);
		else if (FeatureTypeToString(type) == "Rib")
			sprintf_s(buf, "%s%d", "rib", id);
		else if (FeatureTypeToString(type) == "Pocket")
			sprintf_s(buf, "%s%d", "pocket", id);
		else if (FeatureTypeToString(type) == "Shaft")
			sprintf_s(buf, "%s%d", "shaft", id);
		else if (FeatureTypeToString(type) == "Groove")
			sprintf_s(buf, "%s%d", "groove", id);

		std::string feature_name(buf);

		std::string faceName, finalName;
		faceName = GetFaceNameOfCATIA(f);

		finalName = "RSur:(Face:(Brp:("
			+ faceName
			+ ");None:();Cf11:());WithPermanentBody;WithoutBuildError;WithSelectingFeatureSupport;MFBRepVersion_CXR15)\", "
			+ feature_name;

		return finalName;
	}

	// TransCAD Edge�� ���� ��Ī�� �޾Ƽ� CATIA Edge�� ���� ��Ī�� ���
	std::string CATIAReferenceManager::ConvertToBRepEdge(std::string strPersistentName)
	{
		EN_E e;
		Parsing_E(strPersistentName, e);

		TransCAD::FeatureType type;
		int id;
		char buf[1024];

		GetFeatureTypeByTransCADName(e._f2._bn.feature_id, type);
		GetFeatureIDByTransCADName(e._f2._bn.feature_id, id);

		if (e._f2._sn.total_num != 0)
		{
			GetFeatureTypeByTransCADName(e._f2._sn._bn_list[e._f2._sn.total_num - 1].feature_id, type);
			GetFeatureIDByTransCADName(e._f2._sn._bn_list[e._f2._sn.total_num - 1].feature_id, id);
		}

		if (FeatureTypeToString(type) == "Pad")
			sprintf_s(buf, "%s%d", "pad", id);
		else if (FeatureTypeToString(type) == "Rib")
			sprintf_s(buf, "%s%d", "rib", id);
		else if (FeatureTypeToString(type) == "Pocket")
			sprintf_s(buf, "%s%d", "pocket", id);
		else if (FeatureTypeToString(type) == "Shaft")
			sprintf_s(buf, "%s%d", "shaft", id);
		else if (FeatureTypeToString(type) == "Groove")
			sprintf_s(buf, "%s%d", "groove", id);

		std::string feature_name(buf);

		std::string faceName1, faceName2, finalName;
		faceName1 = GetFaceNameOfCATIA(e._f1);
		faceName2 = GetFaceNameOfCATIA(e._f2);

		finalName = "REdge:(Edge:(Face:(Brp:("
			+ faceName1
			+ ");None:();Cf11:());Face:(Brp:("
			+ faceName2
			+ ");None:();Cf11:());None:(Limits1:();Limits2:());Cf11:());WithTemporaryBody;WithoutBuildError;WithSelectingFeatureSupport;MFBRepVersion_CXR14)\", "
			+ feature_name;

		return finalName;
	}

	// TransCAD�� FaceName�� �Է¹޾Ƽ� CATIA�� FaceName�� ����ϴ� �Լ�. SecondaryName ���θ� üũ�Ѵ�.
	std::string CATIAReferenceManager::GetFaceNameOfCATIA(EN_F faceName)
	{
		std::string CATbasicName, CATsecondaryName;

		CATbasicName = GetBasicNameOfCATIA(faceName._bn);

		if (faceName._sn.total_num != 0)
		{
			for (int i = 0; i < faceName._sn.total_num; ++i)
				CATsecondaryName = CATsecondaryName + ";Brp:(" + GetBasicNameOfCATIA(faceName._sn._bn_list[i]) + ")";

			CATbasicName = "(Brp:(" + CATbasicName + ")" + CATsecondaryName + ")";
		}

		return CATbasicName;
	}

	// TransCAD�� BasicName�� �Է¹޾Ƽ� CATIA�� BasicName�� ����ϴ� �Լ�
	std::string CATIAReferenceManager::GetBasicNameOfCATIA(basic_naming basicName)
	{
		// ���� Ÿ�� (-1: ����ġ ��, -2: ����ġ �ݴ��, �� �ܴ� ����)
		int id2 = atoi(basicName.id2.c_str());

		TransCAD::FeatureType type;
		int id;
		char buf[1024];

		GetFeatureTypeByTransCADName(basicName.feature_id, type);
		GetFeatureIDByTransCADName(basicName.feature_id, id);
		sprintf_s(buf, "%s.%d", FeatureTypeToString(type).c_str(), id);
		std::string feature_name(buf); //feature name with number

		sprintf_s(buf, "%s", FeatureTypeToString(type).c_str());
		std::string feature_name1(buf); //feature name without number

		GetFeatureTypeByTransCADName(basicName.id1, type);
		GetFeatureIDByTransCADName(basicName.id1, id);
		sprintf_s(buf, "%s.%d", FeatureTypeToString(type).c_str(), id);
		std::string sketch_name1(buf);

		GetFeatureTypeByTransCADName(basicName.id3, type);
		GetFeatureIDByTransCADName(basicName.id3, id);
		sprintf_s(buf, "%s.%d", FeatureTypeToString(type).c_str(), id);
		std::string sketch_name2(buf);

		int entityID1;
		GetEntityIDByTransCADName(basicName.id1, basicName.id2, entityID1);
		sprintf_s(buf, "%d", entityID1);
		std::string entity_number1(buf);

		int entityID2;
		GetEntityIDByTransCADName(basicName.id3, basicName.id4, entityID2);
		sprintf_s(buf, "%d", entityID2);
		std::string entity_number2(buf);

		std::string CATIAbasicName;

		switch (id2)
		{
		case -1:
			CATIAbasicName = feature_name + ";1";
			break;
		case -2:
			CATIAbasicName = feature_name + ";2";
			break;
		default:
			if (basicName.id3.find("ketch") == -1)
			{
				CATIAbasicName = feature_name
					+ ";0:(Brp:("
					+ sketch_name1		// sketch 
					+ ";"
					+ entity_number1	// sketchEntity 
					+ "))";
			}
			else //Sweep�� ���� Sketch�� 2�� �ִ� ���
			{
				CATIAbasicName = feature_name
					+ ";0:(Brp:("
					+ sketch_name1		// sketch1
					+ ";"
					+ entity_number1	// sketchEntity1
					+ ");Brp:("
					+ sketch_name2		// sketch2
					+ ";"
					+ entity_number2	// sketchEntity2
					+ "))";
			}
			break;
		}

		return CATIAbasicName;
	}

	// referencee �̸��� �޾Ƽ� Feature �̸��� �����ϴ� �Լ�
	std::string CATIAReferenceManager::GetReferenceeFeature(std::string strPersistentName)
	{
		EN_F f;
		Parsing_F(strPersistentName, f);

		TransCAD::FeatureType type;
		char buf[1024];

		GetFeatureTypeByTransCADName(f._bn.feature_id, type);
		sprintf_s(buf, "%s", FeatureTypeToString(type).c_str());
		std::string feature_name(buf);

		return feature_name;
	}

	void CATIAReferenceManager::eew() {
		/*
		::TransCAD::IApplicationPtr g_spApplication;
		::TransCAD::IAssemDocumentPtr _spAssemDocument;
		::TransCAD::IAssemPtr _spAssem;
		::TransCAD::ICompPtr _spComp;

		//COM initialize
		int status = false;
		HRESULT hr = CoInitialize(NULL);
		//TransCAD instance ����
		hr = g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
		//TransCAD initialize error check
		if (FAILED(hr))status = 0;
		//Assembly document ����
		_spAssemDocument = g_spApplication->GetDocuments()->AddAssemDocument();
		//Assem ��ü ����
		_spAssem = _spAssemDocument->GetAssem();
		//Get Parts
		_spComp = _spAssem->GetComponent(1);
		int partCnt = _spComp->GetSize();

		for (int i = 0; i < partCnt; ++i) {
			string partName;
			TransCAD::IPartPtr part = _spComp->GetPart(i);
			cout << "Name of part [" << i << "] in component1" << part->Name << endl;

		}

		TransCAD::IPartPtr _spConstrainedPart;
		_spConstrainedPart = _spComp->GetPart(0);

		TransCAD::IPartPtr _spReferencePart;
		_spReferencePart = _spComp->GetPart(1);

		//Get Contraints
		TransCAD::IStdAssemConstraintsPtr _spContraints;
		_spContraints = _spAssem->Constraints;
		int contraintsCnt = _spContraints->Count;

		//Coaxial constraint

		TransCAD::IStdAssemConstraintPtr _spConstraint = _spContraints->Item[1];
		TransCAD::IStdAssemConstraintCoaxialPtr spCoaxial = _spConstraint;

		string constrainName = spCoaxial->Name;
		TransCAD::StdAssemblyConstraintType constraintType = spCoaxial->Type;
		string constrainedPartName = spCoaxial->ConstrainedPart->Name;
		string constrainedGeomName = spCoaxial->ConstrainedGeometry->GetReferenceeName();
		string referencePartName = spCoaxial->ReferencePart->Name;
		string referenceGeomName = spCoaxial->ReferenceGeometry->GetReferenceeName();



		cout << endl << "<< Coaxial constraint information >>" << endl;
		cout << "Constrained name is [" << constrainName << "]" << endl;
		cout << "Constrained part is [" << constrainedPartName << "]" << endl;
		cout << "Constrained geometry is [" << constrainedGeomName << "[" << endl;
		cout << "COnstraied Part is [" << referencePartName << "]" << endl;
		cout << "Constrained geometry is [ " << referenceGeomName << "]" << endl;

		*/
	}

	//
	//// Sketch �̸��� �Է��ؼ� �� Sketch ���۷����� �̸��� ����
	//std::string CATIAReferenceManager::GetSketchRefBySketchName(std::string _skechName)
	//{
	//	std::string sketchRef;
	//
	//	for (int i = 0; i < (int)_listSketchInfo.size(); ++i) 
	//	{
	//		if(_listSketchInfo[i].sketch_name == _skechName)
	//			sketchRef = _listSketchInfo[i].sketch_ref;
	//	}
	//
	//	return sketchRef;
	//}
}