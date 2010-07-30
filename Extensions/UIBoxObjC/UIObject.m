#import "UIObject.h"


@implementation AEUIObject
-init{
	return [[super init] retain];
}

-retain{
	refcount++;
	return self;
}

-(void)release{
	refcount--;
	if(refcount) return;
	[self dealloc];
}

-(void)dealloc{
	[self free];
}

@end
