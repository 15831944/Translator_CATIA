#include "stdafx.h"
#include "FeatureSOLIDCreatePatternCircular.h"

namespace Post {
	FeatureSOLIDCreatePatternCircular::FeatureSOLIDCreatePatternCircular(Part * pPart, TransCAD::IFeaturePtr spFeature)
		: Feature(pPart, spFeature)
	{
	}

	FeatureSOLIDCreatePatternCircular::~FeatureSOLIDCreatePatternCircular(void)
	{
	}

	void FeatureSOLIDCreatePatternCircular::GetInfo(void)
	{
		TransCAD::IStdSolidOperatePatternCircularFeaturePtr spFeature = GetTransCADFeature();
		TransCAD::IReferencesPtr spTargetFeatures = spFeature->GetTargetFeatures();

		for (int i = 1; i <= spTargetFeatures->GetCount(); ++i)
		{
			TransCAD::IReferencePtr spReference = spTargetFeatures->GetItem(i);

			cout << "	" << "Name           : " << spReference->Name << endl;
			cout << "	" << "ReferenceeName : " << spReference->ReferenceeName << endl;
			cout << "	" << "Type           : " << spReference->Type << endl;
		}

		_featurename = spTargetFeatures->GetItem(1)->GetReferenceeName();

		_isradial = spFeature->IsRadialAnignment ? true : false;
		_gcvt_s(_patternnumber, spFeature->GetAngleNumber(), MAX_DIGITS);
		_gcvt_s(_angleincrement, spFeature->AngleIncrement, MAX_DIGITS);

		// Center


		//���� �κ�. 
		spFeature->GetCenterAxis(&_ptorg[0], &_ptorg[1], &_ptorg[2], &_ptvec[0], &_ptvec[1], &_ptvec[2]);

		//Circular Pattern�� Reference Plane ���ϱ�
		GetCircReferencePlane();

		// TransCAD Ư¡���� �̸����� CATIA Ư¡���� �̸� ���ϱ�
		TransCAD::FeatureType type;
		int _featurenum;

		g_pRefManager->GetFeatureTypeByTransCADName(_featurename, type);
		g_pRefManager->GetFeatureIDByTransCADName(_featurename, _featurenum);

		if (g_pRefManager->FeatureTypeToString(type) == "Pad")
			sprintf_s(_featurenameCATIA, "pad%d", _featurenum);
		else if (g_pRefManager->FeatureTypeToString(type) == "Pocket")
			sprintf_s(_featurenameCATIA, "pocket%d", _featurenum);
		else if (g_pRefManager->FeatureTypeToString(type) == "Rib")
			sprintf_s(_featurenameCATIA, "rib%d", _featurenum);
		else if (g_pRefManager->FeatureTypeToString(type) == "Shaft")
			sprintf_s(_featurenameCATIA, "shaft%d", _featurenum);

		cout << "	" << "Name               : " << _featurename << endl;
		cout << "	" << "Pattern Number     : " << _patternnumber << endl;
		cout << "	" << "Angle Increment    : " << _angleincrement << endl;
		cout << "	" << "Radial Alignment   : " << _isradial << endl;
		cout << "	" << "Center Axis Points : " << "(" << _ptorg[0] << "," << _ptorg[1] << "," << _ptorg[2] << "), (" << _ptvec[0] << "," << _ptvec[1] << "," << _ptvec[2] << ")" << endl;
	}

	void FeatureSOLIDCreatePatternCircular::ToCATIA(void)
	{
		extern int ref_index;

		fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
		fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromName(\"\")\n\n", ref_index);

		ref_index += 1;

		fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
		fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromName(\"\")\n\n", ref_index);

		int CircpatternID = 1;
		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_spFeature->GetName()), CircpatternID))
			return;

		fprintf(GetPart()->file, "Dim circPattern%d As CircPattern\n", CircpatternID);
		fprintf(GetPart()->file, "Set circPattern%d = shapeFactory1.AddNewCircPattern(%s, 1, %s0, 20, %s0, 1, 1, ", CircpatternID, _featurenameCATIA, _patternnumber, _angleincrement);
		fprintf(GetPart()->file, "reference%d, reference%d, True, 0, True)\n\n", ref_index - 1, ref_index);
		fprintf(GetPart()->file, "circPattern%d.CircularPatternParameters = catInstancesandAngularSpacing\n\n", CircpatternID);

		ref_index += 1;

		fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
		fprintf(GetPart()->file, "Set reference%d = originElements1.%s\n\n", ref_index, _refPlane);
		fprintf(GetPart()->file, " circPattern%d.SetRotationAxis reference%d\n\n", CircpatternID, ref_index);

		fprintf(GetPart()->file, "part1.UpdateObject circPattern%d\n\n", CircpatternID);

		ref_index += 1;
	}

	void FeatureSOLIDCreatePatternCircular::GetCircReferencePlane()
	{
		double Vector[3][3];
		Vector[0][0] = 1; Vector[0][1] = 1; Vector[0][2] = 0; //XY Plane ���� ����
		Vector[1][0] = 0; Vector[1][1] = 1; Vector[1][2] = 1; //YZ
		Vector[2][0] = 1; Vector[2][1] = 0; Vector[2][2] = 1; //ZX

		double InnerProduct[3];

		for (int i = 0; i < 3; ++i)
		{
			//���� ����.
			InnerProduct[i] = (_ptvec[0] * Vector[i][0]) + (_ptvec[1] * Vector[i][1]) + (_ptvec[2] * Vector[i][2]);

			//�������� 0�϶�, Ref Plane Ȯ��.
			if (InnerProduct[i] == 0) {
				if (i == 0)
					sprintf_s(_refPlane, "%s", "PlaneXY");
				else if (i == 1)
					sprintf_s(_refPlane, "%s", "PlaneYZ");
				else if (i == 2)
					sprintf_s(_refPlane, "%s", "PlaneZX");
				break;
			}
		}
	}
}