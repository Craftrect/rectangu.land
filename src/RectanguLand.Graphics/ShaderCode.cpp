#include "Precompiled.h"
#include "ShaderCode.h"

namespace Graphics
{
	void ShaderCode::parseGenericBuffer(std::istream& strm) {
		strm.seekg(0);
		strm.read((char*) &mHeader, sizeof(mHeader));

		if (mHeader.numPermutations == 0) {
			throw std::invalid_argument("invalid format in shader file");
		}

		strm.seekg(mHeader.ofsConstTable);
		uint32 cstTable = 0;
		strm.read((char*) &cstTable, sizeof(uint32));
		if (cstTable != 'XBSC') {
			throw std::invalid_argument("invalid format in shader file");
		}

		if (mHeader.nConstTable > 0) {
			mConstTable.resize(mHeader.nConstTable);
			strm.read((char*) mConstTable.data(), mConstTable.size() * sizeof(XBSConstTableEntry));
		}

		for (auto& entry : mConstTable) {
			if (entry.globalId == 0xFFFFFFFF) {
				continue;
			}

			mVariableMap[(ShaderVariables)entry.globalId] = entry;
		}

		strm.seekg(mHeader.ofsPermutationTable);
		mPermutations.resize(1);
		strm.read((char*)mPermutations.data(), sizeof(XBSPermutation));

		strm.seekg(mPermutations[0].ofsCode);
		mShaderData.resize(mPermutations[0].sizeCode);
		strm.read((char*) mShaderData.data(), mShaderData.size());
	}

	bool ShaderCode::getConstEntry(ShaderVariables var, XBSConstTableEntry& centry) const {
		auto itr = mVariableMap.find(var);
		if (itr == mVariableMap.end()) {
			return false;
		}

		centry = itr->second;
		return true;
	}
}