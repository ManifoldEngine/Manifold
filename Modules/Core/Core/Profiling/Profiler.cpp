#include "Profiling.h"

using namespace Mani;

#pragma once

#include <Core/Containers/List.h>
#include <string>

constexpr SizeT MAX_PROFILER = 32;
constexpr SizeT RECORD_CAPACITY = 10'000;

void ThreadProfiler::record(const Profiling::Record& record)
{
	const SizeT index = m_writeCounter % RECORD_CAPACITY;
	m_records[index] = record;
	m_writeCounter++;
	m_publishedCounter.store(m_writeCounter, std::memory_order_release);
}

void ThreadProfiler::read(List<Profiling::Record>& out)
{
	const SizeT readCounter = m_publishedCounter.load(std::memory_order_acquire);

	const SizeT delta = readCounter - m_lastReadCounter;
	if (delta > RECORD_CAPACITY)
	{
		MANI_LOG_WARNING(LogCore, "Profiler recorded more than {} some record might have been lost", RECORD_CAPACITY);
		m_lastReadCounter = readCounter - RECORD_CAPACITY;
	}

	for (SizeT i = m_lastReadCounter; i < readCounter; i++)
	{
		out.add(m_records[i % RECORD_CAPACITY]);
	}

	m_lastReadCounter = readCounter;
}

Profiler::Profiler()
{
	m_lastFrameRecords.reserve(1'000);
}

ThreadProfiler* Profiler::reserveProfiler()
{
	return &m_profilers[m_count++];
}

void Profiler::collectRecords()
{
	m_lastFrameRecords.clear();
	for (auto& profiler : m_profilers)
	{
		profiler.read(m_lastFrameRecords);
	}
}
