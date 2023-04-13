#pragma once

class Time
{
public:
	static Time* Get();

	static void Create();
	static void Delete();

	static bool Stopped() { return isTimerStopped; }
	static float Delta() { return isTimerStopped ? 0.0f : timeElapsed; }

	void Update();
	void Print();

	void Start();
	void Stop();

	float FPS() const { return framePerSecond; }
	float Running() const { return runningTime; }

private:
	Time(void);
	~Time(void);

	static Time* instance;///< �̱��� ��ü

	static bool isTimerStopped;///< Ÿ�̸� ����
	static float timeElapsed;///< ���� ���������κ��� ����ð�


	INT64 ticksPerSecond;///< �ʴ� ƽī��Ʈ
	INT64 currentTime;///< ���� �ð�
	INT64 lastTime;///< �����ð�
	INT64 lastFPSUpdate;///< ������ FPS ������Ʈ �ð�
	INT64 fpsUpdateInterval;///< fps ������Ʈ ����

	UINT frameCount;///< ������ ��
	float runningTime;///< ���� �ð� 1/1000�� ����
	float framePerSecond;///< FPS
};

///////////////////////////////////////////////////////////////////////////////

class Timer
{
public:
	Timer();
	~Timer();

	void Start(std::function<void()> func, int milliSec, UINT repeat = 0);
	void Stop();

private:
	std::mutex m;

	bool bComplete;
	UINT count;
};

///////////////////////////////////////////////////////////////////////////////

class Performance
{
public:
	Performance();

	void Start();
	float End();

private:
	__int64 tick;
	__int64 start, end;
};