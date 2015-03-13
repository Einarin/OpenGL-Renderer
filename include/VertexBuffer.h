#pragma once
#include "VertexAttribBuilder.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <malloc.h>

namespace gl {

	//Shared internal state
	struct VertData {
		bool m_ownsBuffer;
		VertexAttribBuilder m_attrib;
		char* m_buffer; //must be 16 byte aligned
		int m_vertexCount;
		int m_vertSize;
		int m_normOffset;
		int m_tanOffset;
		std::vector<int> m_tcOffset;
		std::vector<int> m_colorOffset;
		std::vector<int> m_boneIdOffset;
		std::vector<int> m_boneWeightOffset;
		VertData() :m_ownsBuffer(false), m_buffer(nullptr)
		{}
#if _MSC_VER <= 1800 //MSVC doesn't generate automatic move constructors :(
		VertData(VertData&& o) : m_ownsBuffer(o.m_ownsBuffer), m_attrib(std::move(o.m_attrib)), m_buffer(o.m_buffer),
			m_vertexCount(o.m_vertexCount), m_vertSize(o.m_vertSize), m_normOffset(o.m_normOffset), m_tanOffset(o.m_tanOffset),
			m_tcOffset(std::move(o.m_tcOffset)), m_colorOffset(std::move(o.m_colorOffset))
		{
			o.m_ownsBuffer = false;
			o.m_buffer = nullptr;
		}
#endif
		void operator=(VertData&& o){
			if(m_ownsBuffer){
				_aligned_free(m_buffer);
			}
			m_ownsBuffer = o.m_ownsBuffer;
			m_attrib = std::move(o.m_attrib);
			m_buffer = o.m_buffer;
			m_vertexCount = o.m_vertexCount;
			m_vertSize = o.m_vertSize;
			m_normOffset = o.m_normOffset;
			m_tanOffset = o.m_tanOffset;
			m_tcOffset = std::move(o.m_tcOffset);
			m_colorOffset = std::move(o.m_colorOffset);
			//finally, clean up the temporary
			o.m_ownsBuffer = false;
			o.m_buffer = nullptr;
		}

		void operator=(const VertData& o){
			//If we don't own our buffer assume the caller knows what they're doing since we don't :)
			if (m_ownsBuffer){
				_aligned_free(m_buffer);
				if (o.m_ownsBuffer){
					//allocate a new buffer and copy the contents
					int bufferSize = o.m_vertSize*o.m_vertexCount;
					m_buffer = (char*)_aligned_malloc(bufferSize,16);
					memcpy(m_buffer, o.m_buffer, bufferSize);
				}
			}
			m_ownsBuffer = o.m_ownsBuffer;
			m_attrib = o.m_attrib;
			m_buffer = o.m_buffer;
			m_vertexCount = o.m_vertexCount;
			m_vertSize = o.m_vertSize;
			m_normOffset = o.m_normOffset;
			m_tanOffset = o.m_tanOffset;
			m_tcOffset = o.m_tcOffset;
			m_colorOffset = o.m_tcOffset;
		}
		~VertData(){
			if (m_ownsBuffer){
				_aligned_free(m_buffer);
			}
		}
	};

	class Vertex{
	protected:
		VertData* m_vbuf;
		int m_index;
	public:
		Vertex(VertData* vbuf, int index) :m_vbuf(vbuf), m_index(index)
		{}
		inline glm::vec3& pos(){
			char* indBase = m_vbuf->m_buffer + (m_index * m_vbuf->m_vertSize);
			return *reinterpret_cast<glm::vec3*>(indBase);
		}
		inline glm::vec3& norm(){
			char* indBase = m_vbuf->m_buffer + (m_index * m_vbuf->m_vertSize);
			return *reinterpret_cast<glm::vec3*>(indBase + m_vbuf->m_normOffset);
		}
		inline glm::vec3& tan(){
			char* indBase = m_vbuf->m_buffer + (m_index * m_vbuf->m_vertSize);
			return *reinterpret_cast<glm::vec3*>(indBase + m_vbuf->m_tanOffset);
		}
		inline glm::vec2& tc2(int index){
			char* indBase = m_vbuf->m_buffer + (m_index * m_vbuf->m_vertSize);
			return *reinterpret_cast<glm::vec2*>(indBase + m_vbuf->m_tcOffset[index]);
		}
		inline glm::vec3& tc3(int index){
			char* indBase = m_vbuf->m_buffer + (m_index * m_vbuf->m_vertSize);
			return *reinterpret_cast<glm::vec3*>(indBase + m_vbuf->m_tcOffset[index]);
		}
		inline glm::vec4& tc4(int index){
			char* indBase = m_vbuf->m_buffer + (m_index * m_vbuf->m_vertSize);
			return *reinterpret_cast<glm::vec4*>(indBase + m_vbuf->m_tcOffset[index]);
		}
		inline glm::vec4& color(int index){
			char* indBase = m_vbuf->m_buffer + (m_index * m_vbuf->m_vertSize);
			return *reinterpret_cast<glm::vec4*>(indBase + m_vbuf->m_colorOffset[index]);
		}
		inline int& boneId(int index) {
			char* indBase = m_vbuf->m_buffer + (m_index * m_vbuf->m_vertSize);
			return *reinterpret_cast<int*>(indBase + m_vbuf->m_boneIdOffset[index]);
		}
		inline float& boneWeight(int index) {
			char* indBase = m_vbuf->m_buffer + (m_index * m_vbuf->m_vertSize);
			return *reinterpret_cast<float*>(indBase + m_vbuf->m_boneWeightOffset[index]);
		}
		inline char* operator&() {
			return m_vbuf->m_buffer + (m_index * m_vbuf->m_vertSize);
		}
	};
	
	class VertexBuffer {
	protected:
		VertData m_data;
		bool m_valid;
	public:
		VertexBuffer(VertData&& data) :m_data(std::move(data)), m_valid(true)
		{}

		//Note this is a move constructor so the class won't get copied by mistake, which will likely result in a double free bug
		VertexBuffer(VertexBuffer&& other) :m_data(std::move(other.m_data)), m_valid(true)
		{
			//make sure nobody can use the old one after this
			other.m_data.m_ownsBuffer = false;
			other.m_data.m_buffer = nullptr;
		}
		//Move assignment operator
		void operator=(VertexBuffer&& other)
		{
			m_data = std::move(other.m_data);
			m_valid = true;
			//make sure nobody can use the old one after this
			other.m_data.m_ownsBuffer = false;
			other.m_data.m_buffer = nullptr;
		}
	
		//Default constructor provided only to let this be stored by value
		VertexBuffer() : m_valid(false)
		{ }
#if _MSC_VER < 1700
	private:
		VertexBuffer(VertexBuffer& other);
	public:
#else
		VertexBuffer(VertexBuffer& other) = delete;
#endif
		//By providing this as a member function rather than a construtor we avoid accidental expensive copy operations
		VertexBuffer duplicate() const{
			VertexBuffer vb;
			//If we're not valid not much to do
			if (m_valid){
				vb.m_valid = m_valid;
				//VertData knows how to copy itself
				vb.m_data = m_data;
				
			}
			return vb;
		}
		
		friend class Vertex;

		inline bool isValid(){
			return m_valid;
		}
		
		inline Vertex operator[](int index){
			if (!m_valid){
				//TODO:DebugBreak();
			}
			return Vertex(&m_data, index);
		}
		//Makes the appropriate OpenGL calls to configure the vertex attributes to match the layout of this buffer
		inline void configureAttributes() const{
			if (m_valid){
				m_data.m_attrib.build();
			}
			else {
				//TODO:DebugBreak();
			}
		}
		inline int vertexCount() const{
			return m_data.m_vertexCount;
		}
		inline int vertSizeBytes() const{
			return m_data.m_vertSize;
		}
		inline int sizeInBytes() const{
			return m_data.m_vertSize * m_data.m_vertexCount;
		}
		//With great power comes great responisibility :)
		char* buffPtr(){
			return m_data.m_buffer;
		}
	};

	class VertexBufferBuilder{
	protected:
		bool m_hasNorm, m_hasTan;
		int m_vertexCount, m_posSize,
			m_tc2Count, m_tc3Count, m_tc4Count, m_colorCount, m_boneCount;
		VertData assemble();
	public:
		inline VertexBufferBuilder() : m_posSize(3), //default to 3D, it's a 3D engine after all
			m_hasNorm(false), m_hasTan(false), m_vertexCount(0),
			m_tc2Count(0), m_tc3Count(0), m_tc4Count(0), m_colorCount(0), m_boneCount(0)
		{}
		inline VertexBufferBuilder& vertexCount(int count){
			m_vertexCount = count;
			return *this;
		}
		inline VertexBufferBuilder& positionSize(int count){
			m_posSize = count;
		}
		inline VertexBufferBuilder& hasNormal(bool hasNorm){
			m_hasNorm = hasNorm;
			return *this;
		}
		inline VertexBufferBuilder& hasTangent(bool hasTan){
			m_hasTan = hasTan;
			return *this;
		}
		//Don't use more than one TC size unless you understand how this class works internally
		inline VertexBufferBuilder& hasTexCoord2D(int tcCount){
			m_tc2Count = tcCount;
			return *this;
		}
		//Don't use more than one TC size unless you understand how this class works internally
		inline VertexBufferBuilder& hasTexCoord3D(int tcCount){
			m_tc3Count = tcCount;
			return *this;
		}
		//Don't use more than one TC size unless you understand how this class works internally
		inline VertexBufferBuilder& hasTexCoord4D(int tcCount){
			m_tc4Count = tcCount;
			return *this;
		}
		inline VertexBufferBuilder& hasVertColor(int colorCount){
			m_colorCount = colorCount;
			return *this;
		}
		inline VertexBufferBuilder& hasBones(int boneCount) {
			m_boneCount = boneCount;
			return *this;
		}
		VertexBuffer build();
		///Expects a pointer aligned to a 16 byte boundary
		///Takes ownership of the passed in buffer
		VertexBuffer useBuffer(char* buffer);
		///Expects a pointer aligned to a 16 byte boundary
		///Does not take ownership of the passed in buffer
		VertexBuffer fillUnownedBuffer(char* buffer);
	};
}