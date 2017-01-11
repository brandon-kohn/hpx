//  Copyright (c) 2017 Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_main.hpp>
#include <hpx/hpx.hpp>
#include <hpx/lcos/channel.hpp>
#include <hpx/runtime/threads/thread.hpp>
#include <hpx/include/iostreams.hpp>

#include <cstdint>

using channel_t = hpx::lcos::local::channel<std::int64_t>;

///////////////////////////////////////////////////////////////////////////////
void skynet_bare(channel_t& c, std::int64_t num, std::int64_t size, std::int64_t div)
{
    if (size != 1)
    {
		channel_t rc;
		for (std::size_t i = 0; i < div; ++i) 
		{
			auto sub_num = num + i * size / div;
			hpx::thread{ [&rc, sub_num, size, div]() { skynet_bare(rc, sub_num, size / div, div); } }.detach();
		}
		std::int64_t sum{ 0 };
		for (std::size_t i = 0; i < div; ++i) 
			sum += rc.get().get();
		c.set(sum);
	}
	else 
		c.set(num);
}

///////////////////////////////////////////////////////////////////////////////
int main()
{
	std::uint64_t t = hpx::util::high_resolution_clock::now();
	channel_t rc;
	skynet_bare(rc, 0, 100000, 10);
	t = hpx::util::high_resolution_clock::now() - t;
	hpx::cout << rc.get().get() << " in " << (t / 1e6) << " ms.\n";
    return 0;
}

