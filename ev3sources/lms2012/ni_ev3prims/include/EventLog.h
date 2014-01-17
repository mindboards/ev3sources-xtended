/**

Copyright (c) 2013 National Instruments Corp.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

SDG
*/

#ifndef EventLog_h
#define EventLog_h

#include "TypeAndDataManager.h"

namespace Vireo {

class EventLog {
private:
    Boolean         _traceEnabled;
    StringRef       _errorLog;
    Int32           _softErrorCount;
    Int32           _hardErrorCount;
    Int32           _warningCount;
    
public:
    enum EventSeverity {
        // Diagnostic trace notice, these are only recorded is tracing is turned on.
        kTrace = 0,
        
        // The data being processed is outside the range expected, but a default interpretation
        // has been defined for the giving situation.
        kWarning = 1,
        
        // Some of the inputs do not make sense but this does not prevent processing remaining data
        kSoftDataError = 2,
        
        // Some of the inputs do not make sense and the function cannot proceed
        kHardDataError = 3,
        
        // Internal state of the system has been copromised
        kAssetFailure = 4,
    };
    
    EventLog(StringRef stringRef);
    Boolean TraceEnabled() { return _traceEnabled; }
    void SetTraceEnabled(Boolean value) { _traceEnabled = value; }
    Int32 TotalErrorCount() { return _softErrorCount + _hardErrorCount; };
    Int32 HardErrorCount() { return  _hardErrorCount; };
    void LogEvent(EventSeverity severity, const char *message, SubString *extra = null);
};



} // namespace Vireo

#endif //EventLog_h
