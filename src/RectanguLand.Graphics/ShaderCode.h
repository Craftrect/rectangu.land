#pragma once

namespace Graphics
{
	enum class ShaderVariables
	{
		cbuff_PerInstance = 0,
		cbuff_PerFrame    = 1,
		cbuff_Matrix      = 2,

		tex_Block		  = 16,
	};

	struct XBSHeader
	{
		uint32 signature;
		uint32 version;
		uint32 shaderType;
		uint32 shaderModel;
		uint32 numPermutations;
		uint32 ofsPermutationTable;
		uint32 nConstTable;
		uint32 ofsConstTable;
	};

	struct XBSConstTableEntry
	{
		uint32 flags;
		uint32 ofsCustomName;
		uint32 lenCustomName;
		uint32 globalId;
		uint32 regNumber;
		uint32 regType;
	};

	struct XBSPermutation
	{
		uint32 signature;
		uint32 nMacros;
		uint32 ofsMacros;
		uint32 sizeCode;
		uint32 ofsCode;
	};

	enum class ShaderCodeType
	{
		Pixel = 0,
		Vertex,
		Geometry,
		Domain,
		Hull,
		MAX
	};

	class ShaderCode
	{
	protected:
		XBSHeader mHeader;

		std::vector<XBSConstTableEntry> mConstTable;
		std::vector<XBSPermutation> mPermutations;
		std::vector<uint8> mShaderData;
		std::map<ShaderVariables, XBSConstTableEntry> mVariableMap;

		void parseGenericBuffer(std::istream& strm);

	public:
		virtual void loadFromBuffer(std::shared_ptr<std::istream> buff) = 0;
		virtual bool isShader(ShaderCodeType type) const { return mHeader.shaderType == (uint32) type; }
		virtual ShaderCodeType getCodeType() const { return (ShaderCodeType) mHeader.shaderType; }

		bool getConstEntry(ShaderVariables variable, XBSConstTableEntry& tableEntry) const;
		const std::vector<uint8>& getShaderData() const { return mShaderData; }
	};

	SHARED_TYPE(ShaderCode);
}