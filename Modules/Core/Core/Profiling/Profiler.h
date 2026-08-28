#pragma once

#include <Core/Containers/List.h>
#include <string>

namespace Mani
{
	constexpr SizeT MAX_PROFILERS = 16;
	constexpr SizeT RECORD_CAPACITY = 1'000;

	namespace Profiling
	{
		struct Record
		{
			std::string_view name = "";
			double duration = 0.0;
		};
	}

	class ThreadProfiler
	{
	public:
		void record(const Profiling::Record& record);
		void read(List<Profiling::Record>& out);
	
	private:
		Mani::Array<Profiling::Record, RECORD_CAPACITY> m_records;

		std::atomic<SizeT> m_publishedCounter = 0;
		SizeT m_lastReadCounter = 0;
		SizeT m_writeCounter = 0;
	};

	class Profiler
	{
	public:
		Profiler();

		ThreadProfiler* reserveProfiler();
		void collectRecords();
		const Mani::List<Profiling::Record>& getLastFrameRecords() const { return m_lastFrameRecords; }
	
	private:
		Mani::Array<ThreadProfiler, MAX_PROFILERS> m_profilers;
		std::atomic<SizeT> m_count = 0;
		Mani::List<Profiling::Record> m_lastFrameRecords;
	};
}