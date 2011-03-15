/*
 * Parallel.h
 *
 *  Created on: 20 Sep 2010
 *      Author: SevenMachines@yahoo.co.uk
 *
 *      Common operations to parallelise operations
 */

#ifndef PARALLEL_H_
#define PARALLEL_H_

#include <map>
#include <boost/uuid/uuid.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

namespace cryomesh {

namespace common {

/**
 * Parallel contains template classes for multithreading and synchronisation
 */
class Parallel {
public:
	Parallel();
	virtual ~Parallel();

	/**
	 * Parallelise update call to a collection of objects up to a thread limit
	 *
	 * @param std::map<boost::uuids::uuid, boost::shared_ptr<T> > & objs
	 * 		A UUID mapped pointer collection
	 * 	@param const int PARALLEL_LIMIT
	 * 		Maximum number of threads to run
	 */
	template<class T>
	static void parallelProcessUpdate(std::map<boost::uuids::uuid,
			boost::shared_ptr<T> > & objs, const int PARALLEL_LIMIT) {
#ifdef __clang__
		boost::thread objs_threads[1];
#else
		boost::thread objs_threads[PARALLEL_LIMIT];
#endif
		//for all in objs
		{
			int objs_index_end = 0;
			int objs_index_start = 0;
			int thread_count = 0;
			typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator
					it_this_objs = objs.begin();
			const typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator
					it_this_objs_end = objs.end();
			while (it_this_objs != it_this_objs_end) {
				if (thread_count < PARALLEL_LIMIT) {
					objs_threads[thread_count]
							= Parallel::getUpdateThread<T>(it_this_objs->second);
					//	std::cout<<"Parallel::parallelProcessUpdate: Bundle: "<<(**it_this_objs).getId() <<std::endl;
					++it_this_objs;
					++objs_index_end;
					++thread_count;
				} else {
					// now wait on all these threads completing
					//for all in objs_threads
					for (int i = 0; i < thread_count; i++) {
						objs_threads[i].join();
					}

					//and reset counters
					objs_index_start = objs_index_end;
					thread_count = 0;
				}

			}
			// now wait on all the last of the threads completing
			for (int i = 0; i < thread_count; i++) {
				objs_threads[i].join();
			}
		}
		//std::cout<<"Parallel::parallelProcessUpdate: END" <<std::endl;
	}

	/**
	 * Generate an threaded update function call to an object
	 *
	 * @param boost::shared_ptr<T> obj
	 * 		Pointer to object containing update() call
	 */
	template <class T>
	static boost::thread getUpdateThread(boost::shared_ptr<T> obj) {
		boost::thread bt(boost::bind(&T::update, obj) );
		return boost::thread(boost::move(bt));
	}
};


}

}

#endif /* PARALLEL_H_ */
