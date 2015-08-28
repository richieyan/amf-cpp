// amftest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "serializer.hpp"
#include "deserializer.hpp"
#include "amfmetaclass.h"
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <iterator>

class VipBean : public amf::AmfMetaClass {

public:
	bool vip;
	int vipLevel;
	VipBean(){
	}
protected:
	void __registerProperties() {
		using namespace amf;
		registerProperty("vip", TypeMarker::BOOL, &vip);
		registerProperty("vipLevel", TypeMarker::INT, &vipLevel);
	}
};

class UserBean : public amf::AmfMetaClass {

public:
	std::string name;
	int64_t uid = 0;
	std::shared_ptr<VipBean> vip;
	std::vector<std::shared_ptr<VipBean>> vipList;
	int pvpScore = 0;
	UserBean() {
		vip = std::make_shared<VipBean>();
	}
protected:
	void __registerProperties() {
		using namespace amf;
		registerProperty("name", TypeMarker::STRING, &name);
		registerProperty("uid", TypeMarker::INT64, &uid);
		registerProperty("vip", TypeMarker::OBJECT, &vip);
		registerProperty("pvpScore", TypeMarker::INT, &pvpScore);
		registerProperty("vipList", TypeMarker::VECTOR_OBJECT, &vipList);
	}

	void __exDeserialize(amf::AmfObject* obj) {
		exDeserializeVector<VipBean>("vipList", obj, vipList);
	}

};


std::shared_ptr<amf::v8> readFile(const std::string & filename) {
	typedef std::istream_iterator<char> istream_iterator;
	std::shared_ptr<amf::v8> ret = std::make_shared<amf::v8>();
	std::ifstream file(filename);
	file >> std::noskipws;

	std::copy(istream_iterator(file), istream_iterator(), std::back_inserter(*ret));

	return ret;
}

int main()
{
	using namespace amf;

	std::shared_ptr<amf::v8> input = readFile("vip.bin");
	std::cout << "read size:" << input->size() << std::endl;
	//Deserializer d;
	//AmfItemPtr item = d.deserialize(*input);

	std::shared_ptr<UserBean> bean = std::make_shared<UserBean>();

	//bean->deserialize(*input);
	
	std::shared_ptr<VipBean> vip1 = std::make_shared<VipBean>();
	vip1->vipLevel = 10;
	vip1->vip = true;

	std::shared_ptr<VipBean> vip2 = std::make_shared<VipBean>();
	vip2->vipLevel = 1;
	vip2->vip = false;

	bean->vipList.push_back(vip1);
	bean->vipList.push_back(vip2);

	//bean->deserialize(*input);

	amf::v8 data = bean->serialize();

	std::shared_ptr<UserBean> bean2 = std::make_shared<UserBean>();
	bean2->deserialize(data);

    return 0;
}

