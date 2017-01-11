//  Copyright (c) 2017 Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/lcos/channel.hpp>
#include <hpx/runtime/threads/thread.hpp>
#include <hpx/include/iostreams.hpp>

using channel_t = hpx::lcos::local::channel<std::uint64_t>;

///////////////////////////////////////////////////////////////////////////////
void skynet_bare(channel_t& c, std::uint64_t num, std::uint64_t size, std::uint64_t div)
{
    if (size != 1)
    {
		channel_t rc;
		for (std::uint64_t i = 0; i < div; ++i) 
		{			
			hpx::thread{ [&rc, num, i, size, div]() 
			{
				auto sd = size / div;
				auto sub_num = num + i * sd;
				skynet_bare(rc, sub_num, sd, div); 
			} }.detach();
		}
		std::uint64_t sum{ 0 };
		for (std::uint64_t i = 0; i < div; ++i) 
			sum += rc.get().get();
		c.set(sum);
	}
	else 
		c.set(num);
}

///////////////////////////////////////////////////////////////////////////////
int hpx_main(boost::program_options::variables_map& vm)
{
	std::uint64_t n_actors = vm["n-actors"].as<std::uint64_t>();
	std::uint64_t t = hpx::util::high_resolution_clock::now();
	channel_t rc;
	skynet_bare(rc, 0, n_actors, 10);
	t = hpx::util::high_resolution_clock::now() - t;
	hpx::cout << rc.get().get() << " in " << (t / 1e6) << " ms.\n";

	return hpx::finalize();
}

int main(int argc, char* argv[])
{
	using namespace boost::program_options;
	options_description desc_commandline(
		"Usage: " HPX_APPLICATION_STRING " [options]");

	desc_commandline.add_options()
		("n-actors,N", value<std::uint64_t>()->default_value(100000),
			"number of actors to spawn in total");

	return hpx::init(desc_commandline, argc, argv);
}
