#include "mesh.h"

#pragma warning(disable:4996)

extern IDirect3DDevice9* g_pDevice;

#define PATH_TO_TEXTURES "resources/meshes/"


Mesh::Mesh()
{
	m_pMesh = NULL;
}


//Mesh::Mesh(LPCWSTR fName)
//{
//	m_pMesh = NULL;
//	Load(fName);
//}

Mesh::Mesh(char fName[])
{
	m_pMesh = NULL;
	Load(fName);
}


Mesh::~Mesh()
{
	Release();
}

HRESULT Mesh::Load(char fName[])
//HRESULT Mesh::Load(LPCWSTR fName)
{
	//release old resource
	Release();
	
	m_white.Ambient = m_white.Specular = m_white.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_white.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	m_white.Power = 1.0f;


	ID3DXBuffer* adjBuffer = NULL;
	ID3DXBuffer* mtrlBuffer = NULL;
	DWORD numMtrl = NULL;
	HRESULT hr = D3DXLoadMeshFromXA(fName, D3DXMESH_MANAGED, g_pDevice, &adjBuffer, &mtrlBuffer, 0, &numMtrl, &m_pMesh);

	if (FAILED(hr))
	{
		::MessageBox(0, L"D3dxLoadMeshFromX Failed", 0, 0);
		return FALSE;
	}

	if (mtrlBuffer != 0 && numMtrl != 0)
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();
		for (int i=0; i<numMtrl;i++)
		{
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;
			m_materials.push_back(mtrls[i].MatD3D);
			if (mtrls[i].pTextureFilename != NULL)
			{
				char textureFileName[90];
				strcpy(textureFileName, PATH_TO_TEXTURES);
				strcat(textureFileName, mtrls[i].pTextureFilename);
				IDirect3DTexture9* tex = 0; 
				D3DXCreateTextureFromFileA(g_pDevice, textureFileName, &tex);
				m_textures.push_back(tex);
			}
			else
			{
				m_textures.push_back(NULL);
			}
		}

		m_pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
			(DWORD*)adjBuffer->GetBufferPointer(), NULL, NULL, NULL);

	}

	adjBuffer->Release();
	mtrlBuffer->Release();
	return hr;
}


void Mesh::Render()
{
	int numMaterials = (int)m_materials.size();
	
	for (int i=0; i<numMaterials; i++)
	{
		if (m_textures[i] != NULL)
		{
			g_pDevice->SetMaterial(&m_white);
		}
		else
		{
			g_pDevice->SetMaterial(&m_materials[i]);
		}

		g_pDevice->SetTexture(0, m_textures[i]);

		m_pMesh->DrawSubset(i);
	}
}


void Mesh::Release()
{
	if (m_pMesh != NULL)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	int numTexture = (int)m_textures.size();

	for (int i=0; numTexture; i++)
	{
		if (m_textures[i] != NULL)
		{
			m_textures[i]->Release();
		}
	}
	m_textures.clear();
	m_materials.clear();
}