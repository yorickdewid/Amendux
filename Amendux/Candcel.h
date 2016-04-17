#pragma once

namespace Amendux {

	class Candcel
	{
	public:
		enum ClientCommand {
			CM_CLIENT_PING = 101, /* Check if mothership is alive */
			CM_CLIENT_SOLICIT = 200, /* Register machine with mothership */
			CM_CLIENT_CHECKIN = 201, /* Checkin with mothership */
			CM_CLIENT_ELIMINATE = 202, /* Detach machine from mothership */
		};

		Candcel() {}
		~Candcel() {}

		void CheckIn();
	};

}
