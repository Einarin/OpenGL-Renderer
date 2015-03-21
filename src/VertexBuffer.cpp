#include "VertexBuffer.h"

namespace gl {
	VertData VertexBufferBuilder::assemble(){
		//Elder wizardry at work here
		//	When adding new data types extra care must be taken
		//	to ensure that alignment isn't violated for any combination of data types
		VertData vbuff;

		int sizeAccum = m_posSize * sizeof(float);
		vbuff.m_attrib.attrib(FLOAT_ATTRIB, m_posSize);
		if (m_hasNorm){
			vbuff.m_normOffset = sizeAccum;
			sizeAccum += 3 * sizeof(float);
			vbuff.m_attrib.attrib(FLOAT_ATTRIB, 3);
		}
		if (m_hasTan){
			vbuff.m_tanOffset = sizeAccum;
			sizeAccum += 3 * sizeof(float);
			vbuff.m_attrib.attrib(FLOAT_ATTRIB, 3);
		}
		int tcCount = m_tc2Count + m_tc3Count + m_tc4Count;
		vbuff.m_tcOffset.resize(tcCount);
		int tcInd = 0;
		for (int i = 0; i < m_tc2Count; i++){
			vbuff.m_tcOffset[tcInd++] = sizeAccum;
			sizeAccum += 2 * sizeof(float);
			vbuff.m_attrib.attrib(FLOAT_ATTRIB, 2);
		}
		for (int i = 0; i < m_tc3Count; i++){
			vbuff.m_tcOffset[tcInd++] = sizeAccum;
			sizeAccum += 3 * sizeof(float);
			vbuff.m_attrib.attrib(FLOAT_ATTRIB, 3);
		}
		for (int i = 0; i < m_tc4Count; i++){
			vbuff.m_tcOffset[tcInd++] = sizeAccum;
			sizeAccum += 4 * sizeof(float);
			vbuff.m_attrib.attrib(FLOAT_ATTRIB, 4);
		}
		vbuff.m_colorOffset.resize(m_colorCount);
		for (int i = 0; i < m_colorCount; i++){
			vbuff.m_colorOffset[i] = sizeAccum;
			sizeAccum += 4 * sizeof(float);
			vbuff.m_attrib.attrib(FLOAT_ATTRIB, 4);
		}
		vbuff.m_boneIdOffset.resize(m_boneCount);
		for (int i = 0; i < m_boneCount; i++) {
			vbuff.m_boneIdOffset[i] = sizeAccum;
			sizeAccum += sizeof(int);
			vbuff.m_attrib.attrib(INT_ATTRIB, 1);
		}
		vbuff.m_boneWeightOffset.resize(m_boneCount);
		for (int i = 0; i < m_boneCount; i++) {
			vbuff.m_boneWeightOffset[i] = sizeAccum;
			sizeAccum += sizeof(float);
			vbuff.m_attrib.attrib(FLOAT_ATTRIB, 1);
		}
		vbuff.m_vertSize = sizeAccum;
		vbuff.m_attrib.setSize(sizeAccum);
		vbuff.m_vertexCount = m_vertexCount;
		return vbuff;
	}
	VertexBuffer VertexBufferBuilder::build(){
		VertData vbuff = assemble();
		char* buffer = (char*)_aligned_malloc(vbuff.m_vertSize*m_vertexCount, 16);
		std::cout << "alloc new " << std::hex << (unsigned int)buffer << std::endl;
		vbuff.m_buffer = buffer;
		vbuff.m_ownsBuffer = true;
		return VertexBuffer(std::move(vbuff));
	}
	VertexBuffer VertexBufferBuilder::useBuffer(char* buffer){
		VertData vbuff = assemble();
		vbuff.m_buffer = buffer;
		vbuff.m_ownsBuffer = true;
		return VertexBuffer(std::move(vbuff));
	}
	VertexBuffer VertexBufferBuilder::fillUnownedBuffer(char* buffer){
		VertData vbuff = assemble();
		vbuff.m_buffer = buffer;
		vbuff.m_ownsBuffer = false;
		return VertexBuffer(std::move(vbuff));
	}
	
} //namespace gl