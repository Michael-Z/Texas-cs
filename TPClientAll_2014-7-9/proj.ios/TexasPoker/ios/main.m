//
//  main.m
//  TexasPoker
//
//  Created by Napoleon on 13-5-6.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//

#import <UIKit/UIKit.h>

int main(int argc, char *argv[]) {
    char acBuffer[1024];
    [[[NSBundle mainBundle] resourcePath] getCString:acBuffer maxLength:1024 encoding:NSASCIIStringEncoding];
    chdir(acBuffer);
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, @"AppController");
    [pool release];
    return retVal;
}
