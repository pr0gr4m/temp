#include "apue.h"
#include <errno.h>
#include <stdarg.h>
#include <syslog.h>

static void log_doit(int, int, int, const char *, va_list ap);

/*
 * 호출자가 반드시 이 변수를 정의해 두어야 한다. 대화식 프로그램이면
 * 1, 데몬이면 0으로 설정하면 된다.
 */
extern int log_to_stderr;

/*
 * 데몬으로 실행되는 경우 syslog 설비를 초기화한다.
 */
void
log_open(const char *ident, int option, int facility)
{
	if (log_to_stderr == 0)
		openlog(ident, option, facility);
}

/*
 * 시스템 호출에 관련된 치명적이지 않은 오류.
 * 시스템의 errno 값을 포함한 메시지를 출력하고 돌아간다.
 */
void
log_ret(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	log_doit(1, errno, LOG_ERR, fmt, ap);
	va_end(ap);
}

/*
 * 시스템 호출에 관련된 치명적 오류.
 * 메시지를 출력하고 종료한다.
 */
void
log_sys(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	log_doit(1, errno, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(2);
}

/*
 * 시스템 호출에 무관한, 치명적이지 않은 오류.
 * 메시지를 출력하고 돌아간다.
 */
void
log_msg(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	log_doit(0, 0, LOG_ERR, fmt, ap);
	va_end(ap);
}

/*
 * 시스템 호출과 무관한 치명적 오류.
 * 메시지를 출력하고 종료한다.
 */
void
log_quit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	log_doit(0, 0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(2);
}

/*
 * 시스템 호출과 관련된 치명적 오류.
 * 호출자가 오류 부호를 명시적으로 지정한다.
 * 메시지를 출력하고 종료한다.
 */
void
log_exit(int error, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	log_doit(1, error, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(2);
}

/*
 * 메시지를 출력하고 호출자에게 돌아간다. errno용 플래그("errnoflag")와
 * 우선순위("priority")를 호출자가 지정한다.
 */
static void
log_doit(int errnoflag, int error, int priority, const char *fmt,
		va_list ap)
{
	char buf[MAXLINE];

	vsnprintf(buf, MAXLINE - 1, fmt, ap);
	if (errnoflag)
		snprintf(buf + strlen(buf), MAXLINE - strlen(buf) - 1, ": %s",
				strerror(error));
	strcat(buf, "\n");

	if (log_to_stderr) {
		fflush(stdout);
		fputs(buf, stderr);
		fflush(stderr);
	} else {
		syslog(priority, "%s", buf);
	}
}
