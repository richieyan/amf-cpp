// amftest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "serializer.hpp"
#include "deserializer.hpp"
#include "amfmetaclass.h"

#include <iostream>
#include <fstream>
#include <iterator>

class VipBean : public amf::AmfMetaClass {

public:
	bool vip;
	int vipLevel;

protected:
	void __registerProperties() {
		using namespace amf;
		registerProperty("vip", TypeMarker::BOOL, &vip);
		registerProperty("vipLevel", TypeMarker::INT, &vipLevel);
	}
};

class UserBean : public amf::AmfMetaClass {

public:
	
	UserBean() {
		vip = std::make_shared<VipBean>();
	}
private:
	std::string name;
	int score;
	std::shared_ptr<VipBean> vip;

protected:
	void __registerProperties() {
		using namespace amf;
		registerProperty("name", TypeMarker::STRING, &name);
		registerProperty("pvpScore", TypeMarker::INT, &score);
		registerProperty("vipInfo", TypeMarker::OBJECT, &vip);
	}
	void __exDeserialize(amf::AmfObject* obj) {
		exDeserialize<VipBean>("vipInfo", obj, vip);
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

	std::shared_ptr<amf::v8> input = readFile("amfobject.bin");
	std::cout << "read size:" << input->size() << std::endl;
	std::shared_ptr<UserBean> bean = std::make_shared<UserBean>();
	bean->deserialize(*input);

	amf::v8 data = bean->serialize();

	std::shared_ptr<UserBean> bean2 = std::make_shared<UserBean>();
	bean2->deserialize(data);

    return 0;
}

