#pragma once

/* <TenativeSock>
 * RAII object for internal use.
 */

namespace SimpleSocksPrivateNS {
	class TenativeSock {
	public:
		TenativeSock();
		~TenativeSock();

		//Creates a socket in 'sock'
		//returns -1 on fail or 0 on success
		//'sock' will be closed by dtor unless
		//'validate()' is called.
		//The args are passed directly to socket().
		//If 'sock' is already a socket the
		//call will fail and return -1.
		//For other failures, the function may
		//be re-called to retry creation.
		int generate(int type, int protocol);

		//Retrieves 'sock'
		__w64 unsigned int get() const;

		//Prevents dtor from closing 'sock'
		//and returns 'sock'.
		__w64 unsigned int validate();
	private:
		__w64 unsigned int sock;
		bool valid;
	};
};
