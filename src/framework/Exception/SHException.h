#ifndef _SH_FRAMEWORK_CLASS_EXCEPTION_H_
#define _SH_FRAMEWORK_CLASS_EXCEPTION_H_

NAMESPACE_SH

    class SHException
    {
        public:
            enum EXCEPTIONCODE
            {
                #include <SHException.code.inc>
            };

        private:
            EXCEPTIONCODE    m_code;
            string           m_msg;
            int32            m_nativeErrorCode;

        public:
            SHException(EXCEPTIONCODE code, const string &msg, int nativeErrorCode = 0): m_code(code), m_msg(msg), m_nativeErrorCode(nativeErrorCode) { }

            EXCEPTIONCODE        code()                const           { return m_code;            }
            const string        &msg()                 const           { return m_msg;             }
            const char          *what()                const           { return m_msg.c_str();     }
            int32                nativeErrorCode()     const           { return m_nativeErrorCode; }
    };

NAMESPACE_SH_END

#endif
