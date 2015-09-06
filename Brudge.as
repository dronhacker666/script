package {
	import flash.external.ExternalInterface;
	import flash.system.*;
	import flash.display.*;
	import flash.events.*;
	import flash.net.*;
	import mx.events.*;
	import flash.utils.describeType;
	import flash.system.Security;

	public class Bridge extends Sprite 
	{
		public function Bridge()
		{
			Security.allowDomain("*"); 

			var url:String = loaderInfo.parameters.src;
			var onReady_callback:String = loaderInfo.parameters.onReady;

			var loader:Loader = new Loader();
			var loaderContext:LoaderContext = new LoaderContext(false, ApplicationDomain.currentDomain, null);

			var urlReq:URLRequest = new URLRequest(url);

			ExternalInterface.addCallback("subscribe", subscribe);

			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, function(event:Event):void{
				log('loaded '+event.target.content);
				var vpaid:Object = event.target.content;

				ExternalInterface.addCallback("handshakeVersion", vpaid.handshakeVersion);
				ExternalInterface.addCallback("initAd", vpaid.initAd);
				ExternalInterface.addCallback("startAd", vpaid.startAd);

				vpaid.addEventListener("AdLoaded", function(event:Event):void{
					log('AdLoaded '+event);
					addChild(Sprite(vpaid));
					vpaid.startAd();
				});
				vpaid.addEventListener("AdError", function(event:Event):void{
					log('AdError '+event);
				});

				log(vpaid.handshakeVersion("2.0"));
				vpaid.initAd(800, 600, "normal", 500, "", "");
			})

			loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, function(event:IOErrorEvent):void{
				log('IOErrorEvent ');
			})

			loader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, function(event:SecurityErrorEvent):void{
				log('SecurityErrorEvent ');
			})

			loader.load(urlReq, loaderContext);
		}

		private function subscribe(type:String, handler:String):void {
			log(type);
			log(handler);
		}

		public function log(msg:String):void
		{
			ExternalInterface.call('log', msg);
		}
	}			
}
