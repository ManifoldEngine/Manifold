#pragma once

#include <Core/Containers/List.h>
#include <string>
#include <limits>

namespace Mani
{
	constexpr SizeT MAX_PROFILERS = 16;
	constexpr SizeT RECORD_CAPACITY = 1'000;

	namespace Profiling
	{
		struct RecordId
		{
			const char* file = "";
			unsigned int line = 0;

			bool operator==(const RecordId& other) const = default;
		};

		constexpr RecordId INVALID_RECORD_ID{ "INVALID", std::numeric_limits<unsigned int>::max() };

		struct Record
		{
			RecordId id = INVALID_RECORD_ID;
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

		ThreadProfiler& getThreadProfiler();
		void collectRecords();
		const Mani::List<Profiling::Record>& getLastFrameRecords() const { return m_lastFrameRecords; }
	
	private:
		ThreadProfiler* reserveProfiler();
		Mani::Array<ThreadProfiler, MAX_PROFILERS> m_profilers;
		std::atomic<SizeT> m_count = 0;
		Mani::List<Profiling::Record> m_lastFrameRecords;
	};
}