#include "pch.h"
#include "CppUnitTest.h"
#include "../LB_11.5.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
	TEST_CLASS(UnitTest1)
	{
	public:
		
        TEST_METHOD(TestFindStudentsWithMinGrade)
        {
            // ϳ�������� �����
            vector<Record> records = {
                {{"Institute1", {{"Faculty1"}, {"Faculty2"}}}, {1, "Group1", "Surname1", 5.0}},
                {{"Institute2", {{"Faculty3"}, {"Faculty4"}}}, {2, "Group2", "Surname2", 4.5}}
            };

            // ������ �������, ��� ������� ������������
            double minGrade = 5.0;
            auto result = findStudentsWithMinGrade(records, 1, "Group1", minGrade);

            // �������� ����������
            Assert::AreEqual(size_t(1), result.size());
            Assert::AreEqual(string("Surname1"), result[0].first);
            Assert::AreEqual(5.0, result[0].second);
        }
	};
}
