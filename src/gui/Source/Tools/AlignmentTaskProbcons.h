#import <Foundation/Foundation.h>
@class ToolAlign;



@interface AlignmentTaskProbcons : NSObject {

    NSTask*                     alignTask;
    NSPipe*                     outputPipe;
    NSFileHandle*               outputFileHandle;
    ToolAlign*                  myAlignmentTool;
}

- (void)alignFile:(id)fInfo;
- (void)alignmentTaskDidFinish:(NSNotification*)notification;
- (id)initWithAlignmentTool:(ToolAlign*)t;

@end
