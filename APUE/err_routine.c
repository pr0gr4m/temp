#include "apue.h"
#include <errno.h>
#include <stdarg.h>

static void err_doit(int, int, const char *, va_list);

/*
 * 시스템 호출에 관련된 치명적이지 않은 오류.
 * 메시지를 출력하고 돌아간다.
 */
void
err_ret(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
}

/*
 * 시스템 호출에 관련된 치명적 오류.
 * 메시지를 출력하고 종료한다.
 */
void
err_sys(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	exit(1);
}

/*
 * 시스템 호출과 무관한, 치명적이지 않은 오류.
 * 호출자가 오류 부호를 명시적으로 지정한다.
 * 메시지를 출력하고 돌아간다.
 */
void
err_cont(int error, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, error, fmt, ap);
	va_end(ap);
}

/*
 * 시스템 호출과 무관한 치명적 오류.
 * 호출자가 오류 부호를 명시적으로 지정한다.
 * 메시지를 출력하고 종료한다.
 */
void
err_exit(int error, const char *fmt, ...)
{
	va_list ap;
	
	va_start(ap, fmt);
	err_doit(1, error, fmt, ap);
	va_end(ap);
	exit(1);
}

/*
 * 시스템 호출과 관련된 치명적 오류.
 * 메시지를 출력하고 코어를 덤프한 후 종료한다.
 */
void
err_dump(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	abort();		/* 코어를 덤프하고 종료 */
	exit(1);		/* 여기에 도달할 수는 없다 */
}

/*
 * 시스템 호출과 무관한, 치명적이지 않은 오류.
 * 메시지를 출력하고 돌아간다.
 */
void
err_msg(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
}

/*
 * 시스템 호출과 무관한 치명적 오류.
 * 호출자가 오류 부호를 명시적으로 지정한다.
 * 메시지를 출력하고 종료한다.
 */
void
err_quit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
	exit(1);
}

/*
 * 메시지를 출력하고 호출자에게 돌아간다.
 * errno용 플래그를 호출자가 지정한ㄷ(errnoflag 매개변수).
 */
void
err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char buf[MAXLINE];
	vsnprintf(buf, MAXLINE - 1, fmt, ap);
	if (errnoflag)
		snprintf(buf + strlen(buf), MAXLINE - strlen(buf) - 1, ": %s",
				strerror(error));
	strcat(buf, "\n");
	fflush(stdout);		/* stdout과 stderr가 같은 경우를 위해 */
	fputs(buf, stderr);
	fflush(NULL);	/* 표준 입출력 라이버르러리의 모든 출력 스트림을 방출 */
}
