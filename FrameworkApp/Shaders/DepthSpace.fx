// HomoHack is the pixel centre offset. It defaults to 0.5, but can be overridden with 0.35
// when doing point sampling as that works better. For this case we are doing point sampling so pass 0.35
float HomogeniseX (int Xc,float HomoHack)
{
	// Convert incoming screen x coordinate into -1 to +1 homogeneous space
	return ((Xc-HomoHack)*(InvWidthF*2.0f))-1.0f;
}

tF32    HomogeniseY (tINT Yc,tF32 HomoHack)
{
// Convert incoming screen y coordinate into -1 to +1 homogeneous space
return 1.0f-((Yc-HomoHack)*(InvHeightF*2.0f));
}
