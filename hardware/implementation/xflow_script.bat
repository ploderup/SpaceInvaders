@ECHO OFF
@REM ###########################################
@REM # Script file to run the flow 
@REM # 
@REM ###########################################
@REM #
@REM # Command line for ngdbuild
@REM #
ngdbuild -p xc6slx45csg324-3 -nt timestamp -bm system.bmm "C:/Users/superman/Desktop/CLGit/SpaceInvaders/hardware/implementation/system.ngc" -uc system.ucf system.ngd 

