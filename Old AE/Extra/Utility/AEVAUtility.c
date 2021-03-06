#include "AEVAUtility.h"
#include "AEInternalRawMesh.h"
/*
void AEVASerializeToIO(AEVA* va,AEIO* io){
	uint64_t version=2;
	version=AENetU64FromHost(version);
	AEIOWrite(io, &version, sizeof(uint64_t));
	
	uint64_t elementCount=va->elementCount;
	elementCount=AENetU64FromHost(elementCount);
	AEIOWrite(io, &elementCount, sizeof(uint64_t));
	
	uint64_t formatbits;
	formatbits=AEVAFormatTo64Bits(& va->format);
	AEIOWrite(io, & formatbits, sizeof(formatbits));
	
	uint32_t* memory=AEVAMap(va, va->elementCount, GL_READ_ONLY);
	
	if(not va->format.indexType) {
		const size_t countsPerVertex=AEVAFormatVertex32Count(& va->format);
		const size_t count=va->elementCount;
		uint32_t ints[countsPerVertex];
		const size_t vertexByteSize=countsPerVertex*sizeof(uint32_t);
		
		for (size_t i=0; i<count; i++) {
			memcpy(ints, memory, vertexByteSize);
			AEVAFormatNetVertexFromHostSwizzle(& va->format, ints);
			memory+=countsPerVertex;
			AEIOWrite(io, ints, vertexByteSize);
		}
	}
	else{
		uint32_t* ints=memory;
		for (size_t i=0; i<va->elementCount; i++) {
			uint32_t value=AENetU32FromHost(ints[i]);
			AEIOWrite(io, &value, sizeof(uint32_t));
		}
	}
	
	AEVAUnmap(va);
}

void AEVAUnserializeFromIO(AEVA* va,AEIO* io){
	uint64_t version;
	AEIORead(io, &version, sizeof(uint64_t));
	version=AEHostU64FromNet(version);
	if(version not_eq 2)
		AEError("Invalid version, only 2 is accepted.");
	AEVADeinit(va);
	memset(va, 0, sizeof(AEVA));
	
	uint64_t length;
	AEIORead(io, &length, sizeof(uint64_t));
	length=AEHostU64FromNet(length);
	
	uint64_t formatbits;
	AEIORead(io, & formatbits, sizeof(formatbits));
	AEVAFormatFrom64Bits(& va->format, formatbits);
	
	uint32_t* memory=AEVAMap(va, length, GL_WRITE_ONLY);
	
	if(not va->format.indexType) {
		const size_t countsPerVertex=AEVAFormatVertex32Count(& va->format);
		const size_t count=va->elementCount;
		uint32_t ints[countsPerVertex];
		const size_t vertexByteSize=countsPerVertex*sizeof(uint32_t);
		
		for (size_t i=0; i<count; i++) {
			memcpy(ints, memory, vertexByteSize);
			memory+=countsPerVertex;
			AEIORead(io, ints, vertexByteSize);
			AEVAFormatHostVertexFromNetSwizzle(& va->format, ints);
		}
	}
	else{
		uint32_t* ints=memory;
		for (size_t i=0; i<va->elementCount; i++) {
			uint32_t value=0;
			AEIORead(io, &value, sizeof(uint32_t));
			ints[i]=AEHostU32FromNet(value);
		}
	}
	
	AEVAUnmap(va);
}*/
/*
void AEVALoadFromObj(AEVA* va, AEVA* ia, const char* objfilename){
	bool hasColors=va->format.hasColors;
	bool hasNormals=va->format.hasNormals;
	int floatcount=hasColors+va->format.textureCoordsPerVertex*2+hasNormals*3+3;
	AEArray(float) vertexList;
	AEArrayInitWithTypeOfSize(&vertexList, sizeof(float)*floatcount);
	AEArray(unsigned int) indexList;
	AEArrayInitWithTypeOfSize(&indexList, sizeof(unsigned int));
	
	AERawMesh* m=AERawMeshLoad(objfilename);
	for(unsigned int i=0;i < m->count.f;i++){
		AERawMeshFace* face=m->f+i;
		for(int j=0;j<3;j++){
			float v[floatcount];
			int size=0;
			if(hasColors){
				unsigned char rgba[4]={255,255,255,255};
				memcpy(v+(size++), rgba, 4);
			}
			for(int k=0;k<(va->format.textureCoordsPerVertex*2);k+=2){
				v[size++]=m->t[face->t[j]].x;
				v[size++]=m->t[face->t[j]].y;
			}
			if(hasNormals){
				v[size++]=m->n[face->n[j]].x;
				v[size++]=m->n[face->n[j]].y;
				v[size++]=m->n[face->n[j]].z;
			}
			v[size++]=m->v[face->v[j]].x;
			v[size++]=m->v[face->v[j]].y;
			v[size++]=m->v[face->v[j]].z;
			if(ia){
				unsigned int index=AEArrayAddBytesUnique(&vertexList, v);
				AEArrayAdd(&indexList, index);
			}
			else AEArrayAddBytes(&vertexList, v);
		}
	}
	AERawMeshDelete(m);

	void* data=NULL;
	
	if(ia){
		data=AEVAMap(ia,AEArrayLength(&indexList),GL_WRITE_ONLY);
		memcpy(data, AEArrayAsCArray(&indexList), AEArrayLengthInBytes(&indexList));
		AEVAUnmap(ia);
	}
	
	data=AEVAMap(va,AEArrayLengthInSizeofType(&vertexList,float),GL_WRITE_ONLY);
	memcpy(data,AEArrayAsCArray(&vertexList),AEArrayLengthInBytes(&vertexList));
	AEVAUnmap(va);
	
	AEArrayDeinit(&indexList);
	AEArrayDeinit(&vertexList);
}
*/

void AEVALoadFromObj(AEVA* va, AEVA* ia, const char* objfilename){
	size_t vertexTypeSize=AEVAFormatByteSize(& va->format);
	size_t indexTypeSize=ia ? AEVAFormatByteSize(& ia->format) : 0;
	AEArray(void) vertexList;
	AEArrayInitWithTypeOfSize(&vertexList, vertexTypeSize);
	AEArray(GLuint) indexList;
	AEArrayInit(& indexList);
	
	AERawMesh* m=AERawMeshLoad(objfilename);
	for(uint64_t i=0;i < m->count.f;i++){
		AERawMeshFace* face=m->f+i;
		for(int j=0;j<3;j++){
			uint8_t buffer[vertexTypeSize];
			uint8_t* data=buffer;
			AERGBA color={255,255,255,255};
			AEVec2 ts[va->format.textureCoordsPerVertex];
			for(int k=0; k<va->format.textureCoordsPerVertex; k++) ts[k]=m->t[face->t[j]];
			AEVAFormatSetVertex(& va->format, buffer, ts, &color, m->n+face->n[j], m->v+face->v[j]);
			if(ia){
				uint64_t index=AEArrayAddBytesUnique(&vertexList, data);
				AEArrayAdd(&indexList, index);
			}
			else AEArrayAddBytes(&vertexList, data);
		}
	}
	
	AERawMeshDelete(m);
	
	void* data=NULL;
	
	if(ia){
		data=AEVAMap(ia,AEArrayLength(&indexList),GL_WRITE_ONLY);
		for (uint64_t i=0; i<AEArrayLength(& indexList); i++) {
			AEVAFormatSetIndex(& ia->format, ((uint8_t*)data)+i*indexTypeSize, AEArrayAsCArray(&indexList)[i]);
		}
	//	memcpy(data, AEArrayAsCArray(&indexList), AEArrayLengthInBytes(&indexList));
		AEVAUnmap(ia);
	}
	
	data=AEVAMap(va,AEArrayLength(&vertexList),GL_WRITE_ONLY);
	memcpy(data,AEArrayAsCArray(&vertexList),AEArrayLengthInBytes(&vertexList));
	AEVAUnmap(va);
	
	AEArrayDeinit(&indexList);
	AEArrayDeinit(&vertexList);
}