//#include <iostream>
//#include <boost/thread.hpp>
//
//using namespace std;
//
//class CSampleIO
//{
//public:
//	void TestThread()
//	{
//		while (true)                //�Ϲ������� while���� ���������� ���Ѵٰ� �����ϱ� ��� ���⼭�� �׷��� ����. Sleep���� ���� ���� �ٸ� �����尡 ���� �� �� �ִ� ��ȸ�� ����� ��. 
//		{
//			cout << "1 ";
//			boost::this_thread::sleep(boost::posix_time::millisec(500));
//		}
//	}
//
//	void TestThreadSecond(int num)
//	{
//		while (true)
//		{
//			cout << num << " ";
//			boost::this_thread::sleep(boost::posix_time::millisec(500));
//		}
//	}
//
//	void TestThreadThird(int num, int num2)
//	{
//		while (true)
//		{
//			cout << num << " ";
//			boost::this_thread::sleep(boost::posix_time::millisec(500));
//		}
//	}
//};
//
//int main()
//{
//	//��ü����
//	CSampleIO io;
//
//	//���������(���ڴ� ��� �߰� ��ų �� ����)
//
//	boost::thread th1 = boost::thread(boost::bind(&CSampleIO::TestThread, &io));
//	boost::thread th2 = boost::thread(boost::bind(&CSampleIO::TestThreadSecond, &io, 2));
//	boost::thread th3 = boost::thread(boost::bind(&CSampleIO::TestThreadThird, &io, 3, NULL));
//
//	//join���� �ش� �����尡 ���� ��
//	th1.join();
//
//	th2.join();
//
//	return 0;
//}


//Boost Example
//#include "stdio.h"  
//#include "string"  
//#include "boost/thread.hpp"  
//#include "boost/bind.hpp"  
//#include <windows.h>  
//class Man
//{
//public:
//	std::string name_;
//	Man(const char *name)
//	{
//		name_ = name;
//	}
//
//	void Say(const char *msg)
//	{
//		printf("[%d] %s: %s\n", GetCurrentThreadId(), name_.c_str(), msg);
//	}
//};
//
//int fn(int a, int b, int c, int d, int e)
//{
//	printf("a = %d, b = %d, c = %d, d = %d, e = %d\n", a, b, c, d, e);
//	return a + b + c + d + e;
//}
//
//
//int main()
//{
//	Man bob("Bob");
//	Man mark("Mark");
//
//	boost::thread th1(boost::bind(&Man::Say, bob, "Hello"));
//	boost::thread th2(boost::bind(&Man::Say, mark, "Bye~"));
//	boost::thread th3(boost::bind(&Man::Say, mark, "Bye~"));
//	boost::thread th4(boost::bind(&Man::Say, mark, "Bye~"));
//	boost::thread th5(boost::bind(&Man::Say, mark, "Bye~"));
//	boost::thread th6(boost::bind(&Man::Say, mark, "Bye~"));
//	boost::thread th7(boost::bind(&Man::Say, mark, "Bye~"));
//	boost::thread th8(boost::bind(&Man::Say, mark, "Bye~"));
//
//	printf("%d\n", boost::bind(fn, 1, 2, 3, 4, 5)());
//
//	printf("%d\n", boost::bind(fn, 1, 2, _1, 4, 5)(10));
//	printf("%d\n", boost::bind(fn, 1, 2, 3, _1, 5)(50));
//	printf("%d\n", boost::bind(fn, 1, 2, 3, _1, 5)(50));
//	printf("%d\n", boost::bind(fn, 1, 2, 3, _1, 5)(50));
//	printf("%d\n", boost::bind(fn, 1, 2, 3, _1, 5)(50));
//	printf("%d\n", boost::bind(fn, 1, 2, 3, _1, 5)(50));
//	printf("%d\n", boost::bind(fn, 1, 2, 3, _1, 5)(50));
//
//	th1.join();
//	th2.join();
//
//	system("pause");
//	return 0;
//}