#include"cond.h"
#include"read_mutex.h"
using namespace std;
//线程监控的模板类，通常线程锁，都通过该类来使用，而不是直接使用TC_ThreadMutex和TC_ThreadRecMutex
//该类将TC_ThreadMutex/TC_ThreadRecMutex与TC_ThreadCond结合起来
namespace tars{
	template<class T,class P>
	class TC_Monitor{
	public:
		//定义锁控制对象
		typedef TC_LockT<TC_Monitor<T,P> > Lock;
		typedef TC_TryLockT<TC_Monitor<T,P> > TryLock;
		//构造函数
		TC_Monitor():_nnotify(0){}
		//析构函数
		virtual ~TC_Monitor(){}
		//上锁,上锁后要将_nnotify置零
		void lock() const {
			_mutex.lock();
			_nnotify = 0;
		}
		//解锁，根据上锁次数的通知
		void unlock() const {
			notifyImpl(_nnotify);
			_mutex.unlock();
		}
		//尝试锁
		bool trylock() const{
			bool result = _mutex.trylock();
			if(result)
				_nnotify = 0;
			return result;
		}
		//等待，当前调用线程在锁上等待，直到事件通知
		void wait() const {
			notifyImpl(_nnotify);
			try{
				_cond.wait(_mutex);
			}
			catch(...){
				_nnotify = 0;
				throw;
			}
			_nnotify = 0;
		}
	protected:
		//通知实现
		void notifyImpl(int nnotify) const {
			if(nnotify != 0)
				if(nnotify == -1){
					//通知所有等待在该锁上的所有线程醒过来、
					_cond.broadcast();
					return;
				}
				else{
					while(nnotify > 0){
						_cond.signal();
						--nnotify;
					}
				}
		}

	private:
		//上锁的次数
		mutable int _nnotify;
		mutable P _cond;
		T _mutex;
	};
}
