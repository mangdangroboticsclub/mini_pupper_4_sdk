import unittest
from unittest.mock import Mock

from mpx_tool.sdk.gateway import GatewayClient


class GatewayPublishTests(unittest.TestCase):
    def setUp(self):
        self.client = GatewayClient("https://gateway.example.com")

    def publish(self, **kwargs):
        self.client._request_with_status = Mock(return_value=(201, {}))
        result = self.client.publish(
            skill_id="dev~dance",
            title="Dance",
            version="1.0.0",
            artifact_b64="YQ==",
            manifest={"tags": ["dance"]},
            token="token",
            skill_type="WASM",
            source_language="c",
            **kwargs,
        )
        return result, self.client._request_with_status

    def test_publish_omits_pricing_when_not_requested(self):
        _, request = self.publish()
        body = request.call_args.kwargs["data"]
        self.assertNotIn("price_minor", body)
        self.assertNotIn("currency", body)

    def test_publish_sends_integer_minor_units(self):
        _, request = self.publish(price_minor=100, currency="JPY")
        body = request.call_args.kwargs["data"]
        self.assertEqual(body["price_minor"], 100)
        self.assertEqual(body["currency"], "JPY")

    def test_visibility_is_authenticated_and_owner_scoped_by_route(self):
        self.client._request_with_status = Mock(return_value=(200, {"visibility": "public"}))
        self.client.set_visibility("dev~a skill", "public", "token")
        method, path = self.client._request_with_status.call_args.args[:2]
        kwargs = self.client._request_with_status.call_args.kwargs
        self.assertEqual(method, "PATCH")
        self.assertEqual(path, "/v1/skills/dev~a%20skill/visibility")
        self.assertEqual(kwargs["data"], {"visibility": "public"})
        self.assertEqual(kwargs["token"], "token")


if __name__ == "__main__":
    unittest.main()
