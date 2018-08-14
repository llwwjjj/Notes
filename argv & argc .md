# Notes
- argv与argc相关
-----------------
*概念
  - int main(int argc,char *argv[]){...}
  - argc的值永远不小于1，如果用户在命令行键入实参，argc就有较高的计数值。argv至少含有一个char指针，这个指针指向程序的可执行文件名，如果用户在命令行键入实参，argv[1],argv[2]等将分别指向对应的实参，多个实参用空格分开。如：
  ```cpp
  #include <iostream>
  using namespace std;
  int main(int argc,char *argv[]){
        cout << "argc = "<<argc<<endl;
        int i;
        for(i = 0;i < argc;++i)
                cout<<"argv["<<i<<"]"<<" = "<<argv[i]<<endl;
        return 0;
  }
  ```
  - 当键入./a.out时，会得到argc = 1,argv [0] = a.out
  - 当键入./a.out qwe ce 123c时，会得到argc = 4，argv[0] = a.out，argv[1] = qwe，argv[2] = ce，argv[3] = 123c
  
