#include<string>
#include<stdexcept>
#include<cerrno>
#include"tc_ex.h"
//鎖類,尚有疑問：_mutex是const引用，而它調用lock，trylock和unlock應該是不行的
using namespace std;
namespace tars{
	//鎖異常
	struct TC_Lock_Exception : public TC_Exception{
		TC_Lock_Exception(const string &buffer):TC_Exception(buffer){}
		TC_Lock_Exception(const string &buffer,int err):TC_Exception(buffer,err){}
		~TC_Lock_Exception()throw(){}
	};

	//構造時加鎖，析構時解鎖
	template<class T>
	class TC_LockT{
	public:
		//構造
		TC_LockT(const T &mutex):_mutex(mutex){
			_mutex.lock();
			_acquired = true;
		}
		//析構
		virtual ~TC_LockT(){
			if(_acquired)
				_mutex.unlock();
		}
		//嘗試上鎖，成功返回true
		bool tryAcquire() const{
			_acquired = _mutex.tryLock();
			return _acquired;
		}
		//釋放鎖，如果沒有上過鎖則拋出異常
		void release() const{
			if(!_acquired)
				throw TC_Lock_Exception("thread hasn't been locked!");
			_mutex.unlock();
			_acquired = false;
		}
		//是否已經上鎖
		bool acquired() const{
			return _acquired;
		}
	protected:
		//引用類型的成員變量的初始化必須在構造函數的初始化列表中進行初始化
		const T & _mutex;
		//是否已經上鎖
		mutable bool _acquired;
		//用於鎖嘗試操作的構造函數		
		TC_LockT(const T& mutex,bool):_mutex(mutex){
			_acquired = _mutex.tryLock();
		}
	private:
		//拷貝構造函數，禁止拷貝
		TC_LockT(const TC_LockT&);
		//拷貝賦值運算符，禁止拷貝
		TC_LockT& operator=(const TC_LockT&);
	};

	//嘗試上鎖
	template<class T>
	class TC_TryLockT : public TC_LockT<T>{
	public:
		TC_TryLockT(const T& mutex):TC_LockT<T>(mutex,true){}
	};

	//空鎖
	class TC_EmptyMutex{
	public:
		int lock() const {return 0;}
		int unlock() const {return 0;}
		bool trylock() const {return true;}
	};

	//讀寫鎖中讀鎖模板類
	template<class T>
	class TC_RW_RLockT{
	public:
		//構造時加鎖
		TC_RW_RLockT(T& lock):_rwLock(lock),_acquired(false){
			_rwLock.ReadLock();
			_acquired = true;
		}
		//析構時解鎖
		~TC_RW_RLockT(){
			if(_acquired)
				_rwLock.Unlock();
		}
	private:
		//讀寫鎖對象
		const T& _rwLock;
		//是否已經上鎖
		mutable bool _acquired;
		//禁止拷貝
		TC_RW_RLockT(const TC_RW_RLockT&);
		TC_RW_RLockT& operator=(const TC_RW_RLockT&);
	};

	//讀寫鎖中寫類模板類
	template<class T>
	class TC_RW_WLockT{
	public:
		//構造時加鎖
		TC_RW_WLock(T& lock):_rwLock(lock),_acquired(false){
			_rwLock.WriteLock();
			_acquired = true;
		}
		//析構時解鎖
		~TC_RW_WLock(){
			if(_acquired)
				_rwLock.Unlock();
		}
	private:
		//讀寫鎖對象
		const T& _rwLock;
		//是否已經上鎖
		mutable bool _acquired;
		//禁止拷貝
		TC_RW_WLockT(const TC_RW_WLockT&);
		TC_RW_WLockT& operator=(const TC_RW_WLockT&);
	};
}