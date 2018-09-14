# Notes
- Boost相关随笔，学习记录
-------------
目录
-------------
* boost::program_options 
  - program_options是一系列pair<name,value>组成的选项列表，它允许程序通过命令行或配置文件来读取这些参数选项
  - program_options主要通过下面三个组件完成：
     - 选项描述器 options_description：描述当前的程序定义了哪些选项
     - 选项分析器 parse_command_line：解析由命令行输入的参数
     - 选项存储器 variable_map：容器，用于存储解析后的选项
  - 1.构造选项描述器和选项存储器
     ```cpp
      namespace po = boost::program_options;
      po::options_description opts("all options");
      po::variables_map vm;
     ```
  - 2.为选项描述器增加选项，其参数是key，value类型
     ```cpp
      opts.add_options()
      ("filename",po::value<string>(),"the file name which want to be found")
      ("help", "this is a program to find a specified file");
     ```
  - 3.解析命令行参数，然后将其存入选项存储器，如果输入了未定义的选项，程序会抛出异常，所以用try-catch
     ```cpp
      try{
        po::store(po::parse_commond_line(argc,argv,opts),vm);
      }
      catch(...){
        cerr<<...;
        return 0;
      }
     ```
  - 4.处理实际信息
     ```cpp
      if(vm.count("help")){//如果有参数中有help选项
        cout<<opts<<endl;
      }
      if(vm.count("filename")){
        cout << "find " << vm["filename"].as<std::string>() << std::endl;
      }
      if(vm.empty()){
        cout<<"no options found \n"<<endl;
      }
      ```
      
