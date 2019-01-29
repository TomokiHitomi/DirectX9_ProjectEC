xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 12;
 -0.00014;0.70700;-0.00009;,
 -0.00009;0.70700;-0.00014;,
 0.00000;0.00000;-0.70711;,
 -0.70711;0.00000;0.00000;,
 -0.00000;-0.70700;-0.00000;,
 -0.00000;-0.70700;-0.00000;,
 0.00014;0.70700;0.00009;,
 0.70711;0.00000;0.00000;,
 0.00000;-0.70700;0.00000;,
 0.00009;0.70700;0.00014;,
 0.00000;0.00000;0.70711;,
 0.00000;-0.70700;0.00000;;
 
 10;
 4;0,1,2,3;,
 4;3,2,4,5;,
 4;1,6,7,2;,
 4;2,7,8,4;,
 4;6,9,10,7;,
 4;7,10,11,8;,
 4;9,0,3,10;,
 4;10,3,5,11;,
 4;9,6,1,0;,
 4;5,4,8,11;;
 
 MeshMaterialList {
  1;
  10;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
 MeshNormals {
  14;
  -0.325083;0.888055;-0.325083;,
  -0.577384;0.577283;-0.577384;,
  -0.325036;-0.888089;-0.325036;,
  0.325044;0.888084;-0.325044;,
  0.577333;0.577385;-0.577333;,
  0.325036;-0.888090;-0.325036;,
  0.325083;0.888055;0.325083;,
  0.577384;0.577283;0.577384;,
  0.325036;-0.888089;0.325036;,
  -0.577323;-0.577405;-0.577323;,
  0.577322;-0.577407;-0.577322;,
  0.577323;-0.577405;0.577323;,
  -0.577333;0.577385;0.577333;,
  -0.577322;-0.577407;0.577322;;
  10;
  4;0,0,1,1;,
  4;9,9,2,2;,
  4;4,3,4,4;,
  4;10,10,5,10;,
  4;7,6,7,7;,
  4;11,11,8,11;,
  4;12,12,12,12;,
  4;13,13,13,13;,
  4;6,3,0,0;,
  4;2,2,5,8;;
 }
}
